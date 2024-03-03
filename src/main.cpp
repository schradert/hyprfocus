#define WLR_USE_UNSTABLE

#include <hyprland/src/plugins/PluginAPI.hpp>

#include <hyprland/src/debug/Log.hpp>

inline HANDLE PHANDLE = nullptr;

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/Window.hpp>
#include <hyprland/src/managers/AnimationManager.hpp>

#include "Flash.hpp"
#include "Shrink.hpp"

#include <iostream>

CWindow *g_pPreviouslyFocusedWindow = nullptr;
bool g_bMouseWasPressed = false;

std::unordered_map<std::string, std::unique_ptr<IFocusAnimation>> g_mAnimations;

void flashWindow(CWindow *pWindow) {
  Debug::log(LOG, "[hyprfocus] Flashing window");
  static auto *const configValue =
      (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:hyprfocus:focus_animation")
          ->getDataStaticPtr();
  Debug::log(LOG, "[hyprfocus] Animation: {}", *configValue);

  auto it = g_mAnimations.find(*configValue);
  if (it != g_mAnimations.end()) {
    Debug::log(LOG, "[hyprfocus] Calling onWindowFocus for animation {}",
               *configValue);
    it->second->onWindowFocus(pWindow, PHANDLE);
  }
}

void flashCurrentWindow(std::string) {
  Debug::log(LOG, "[hyprfocus] Flashing current window");
  static auto *const PHYPRFOCUSENABLED =
      (Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:hyprfocus:enabled")
          ->getDataStaticPtr();
  if (!*PHYPRFOCUSENABLED) {
    Debug::log(LOG, "[hyprfocus] HyprFocus is disabled");
    return;
  }

  if (g_pPreviouslyFocusedWindow == nullptr) {
    Debug::log(LOG, "[hyprfocus] No previously focused window");
    return;
  }

  flashWindow(g_pPreviouslyFocusedWindow);
}

static void onActiveWindowChange(void *self, std::any data) {
  try {
    Debug::log(LOG, "[hyprfocus] Active window changed");
    auto *const PWINDOW = std::any_cast<CWindow *>(data);
    static auto *const PHYPRFOCUSENABLED =
        (Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
            PHANDLE, "plugin:hyprfocus:enabled")
            ->getDataStaticPtr();

    if (!*PHYPRFOCUSENABLED) {
      Debug::log(LOG, "[hyprfocus] HyprFocus is disabled");
      return;
    }

    if (PWINDOW == nullptr) {
      Debug::log(LOG, "[hyprfocus] Window is null");
      return;
    }

    if (PWINDOW == g_pPreviouslyFocusedWindow) {
      Debug::log(LOG, "[hyprfocus] Window is the same as the previous one");
      return;
    }

    flashWindow(PWINDOW);
    g_pPreviouslyFocusedWindow = PWINDOW;

  } catch (std::bad_any_cast &e) {
    Debug::log(ERR, "[hyprfocus] Cast Error: {}", e.what());
  } catch (std::exception &e) {
    Debug::log(ERR, "[hyprfocus] Error: {}", e.what());
  }
}

static void onMouseButton(void *self, std::any data) {
  try {
    auto *const PWLRMOUSEBUTTONEVENT =
        std::any_cast<wlr_pointer_button_event *>(data);
    g_bMouseWasPressed = PWLRMOUSEBUTTONEVENT->state == WLR_BUTTON_PRESSED;

  } catch (std::bad_any_cast &e) {
    Debug::log(ERR, "[hyprfocus] Cast Error: {}", e.what());
  } catch (std::exception &e) {
    Debug::log(ERR, "[hyprfocus] Error: {}", e.what());
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
    Debug::log(LOG, "[hyprfocus] Registered animation: {}", name);
  }

  HyprlandAPI::reloadConfig();
  Debug::log(LOG, "[hyprfocus] Reloaded config");

  HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "activeWindow",
      [&](void *self, SCallbackInfo &info, std::any data) {
        onActiveWindowChange(self, data);
      });
  Debug::log(LOG, "[hyprfocus] Registered active window change callback");
  HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "mouseButton",
      [&](void *self, SCallbackInfo &info, std::any data) {
        onMouseButton(self, data);
      });
  Debug::log(LOG, "[hyprfocus] Registered mouse button callback");

  return {"hyprfocus", "animate windows on focus", "Vortex", "2.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {}
