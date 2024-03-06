#define WLR_USE_UNSTABLE

#include "Globals.hpp"
#include <hyprland/src/plugins/PluginAPI.hpp>

#include <hyprland/src/debug/Log.hpp>

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/Window.hpp>
#include <hyprland/src/managers/AnimationManager.hpp>

#include "Flash.hpp"
#include "Shrink.hpp"

CWindow *g_pPreviouslyFocusedWindow = nullptr;
bool g_bMouseWasPressed = false;

std::unordered_map<std::string, std::unique_ptr<IFocusAnimation>> g_mAnimations;

void flashWindow(CWindow *pWindow) {
  static const auto *focusAnimation =
      (Hyprlang::STRING const *)(HyprlandAPI::getConfigValue(
                                     PHANDLE,
                                     "plugin:hyprfocus:focus_animation")
                                     ->getDataStaticPtr());
  hyprfocus_log(LOG, "Flashing window");
  hyprfocus_log(LOG, "Animation: {}", *focusAnimation);

  auto it = g_mAnimations.find(*focusAnimation);
  if (it != g_mAnimations.end()) {
    hyprfocus_log(LOG, "Calling onWindowFocus for animation {}",
                  *focusAnimation);
    it->second->onWindowFocus(pWindow, PHANDLE);
  }
}

void flashCurrentWindow(std::string) {
  hyprfocus_log(LOG, "Flashing current window");
  static auto *const PHYPRFOCUSENABLED =
      (Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:hyprfocus:enabled")
          ->getDataStaticPtr();
  if (!*PHYPRFOCUSENABLED) {
    hyprfocus_log(LOG, "HyprFocus is disabled");
    return;
  }

  if (g_pPreviouslyFocusedWindow == nullptr) {
    hyprfocus_log(LOG, "No previously focused window");
    return;
  }

  flashWindow(g_pPreviouslyFocusedWindow);
}

static void onActiveWindowChange(void *self, std::any data) {
  try {
    hyprfocus_log(LOG, "Active window changed");
    auto *const PWINDOW = std::any_cast<CWindow *>(data);
    static auto *const PHYPRFOCUSENABLED =
        (Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
            PHANDLE, "plugin:hyprfocus:enabled")
            ->getDataStaticPtr();

    if (!**PHYPRFOCUSENABLED) {
      hyprfocus_log(LOG, "HyprFocus is disabled");
      return;
    }

    if (PWINDOW == nullptr) {
      hyprfocus_log(LOG, "Window is null");
      return;
    }

    if (PWINDOW == g_pPreviouslyFocusedWindow) {
      hyprfocus_log(LOG, "Window is the same as the previous one");
      return;
    }

    flashWindow(PWINDOW);
    g_pPreviouslyFocusedWindow = PWINDOW;

  } catch (std::bad_any_cast &e) {
    hyprfocus_log(ERR, "Cast Error: {}", e.what());
  } catch (std::exception &e) {
    hyprfocus_log(ERR, "Error: {}", e.what());
  }
}

static void onMouseButton(void *self, std::any data) {
  try {
    auto *const PWLRMOUSEBUTTONEVENT =
        std::any_cast<wlr_pointer_button_event *>(data);
    g_bMouseWasPressed = PWLRMOUSEBUTTONEVENT->state == WLR_BUTTON_PRESSED;

  } catch (std::bad_any_cast &e) {
    hyprfocus_log(ERR, "Cast Error: {}", e.what());
  } catch (std::exception &e) {
    hyprfocus_log(ERR, "Error: {}", e.what());
  }
}

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;

  HyprlandAPI::addConfigValue(PHANDLE, "plugin:hyprfocus:enabled",
                              Hyprlang::INT{0});
  HyprlandAPI::addConfigValue(PHANDLE, "plugin:hyprfocus:focus_animation",
                              Hyprlang::STRING("flash"));

  HyprlandAPI::addDispatcher(PHANDLE, "animatefocused", &flashCurrentWindow);

  g_mAnimations["flash"] = std::make_unique<CFlash>();
  g_mAnimations["shrink"] = std::make_unique<CShrink>();
  g_mAnimations["none"] = std::make_unique<IFocusAnimation>();

  for (auto &[name, pAnimation] : g_mAnimations) {
    pAnimation->init(PHANDLE, name);
    hyprfocus_log(LOG, "Registered animation: {}", name);
  }

  HyprlandAPI::reloadConfig();
  g_pConfigManager->tick();
  hyprfocus_log(LOG, "Reloaded config");

  // Declare globals
  // static const auto *focusAnimation =
  //     (Hyprlang::STRING const *)(HyprlandAPI::getConfigValue(
  //                                    PHANDLE,
  //                                    "plugin:hyprfocus:focus_animation")
  //                                    ->getDataStaticPtr());
  // g_sAnimationType = *focusAnimation;

  // Setup animations
  for (auto &[name, pAnimation] : g_mAnimations) {
    pAnimation->setup(PHANDLE, name);
    hyprfocus_log(LOG, "Setup animation: {}", name);
  }

  // Register callbacks
  HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "activeWindow",
      [&](void *self, SCallbackInfo &info, std::any data) {
        onActiveWindowChange(self, data);
      });
  hyprfocus_log(LOG, "Registered active window change callback");

  HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "mouseButton",
      [&](void *self, SCallbackInfo &info, std::any data) {
        onMouseButton(self, data);
      });
  hyprfocus_log(LOG, "Registered mouse button callback");

  HyprlandAPI::reloadConfig();

  return {"hyprfocus", "animate windows on focus", "Vortex", "2.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {}
