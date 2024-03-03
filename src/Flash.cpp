#include "Flash.hpp"

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/Window.hpp>
#include <hyprland/src/managers/AnimationManager.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

void CFlash::init(HANDLE pHandle, std::string animationName) {
  IFocusAnimation::init(pHandle, animationName);
  addConfigValue(pHandle, "flash_opacity", Hyprlang::FLOAT{0.5f});
}

void CFlash::onWindowFocus(CWindow *pWindow, HANDLE pHandle) {
  Debug::log(LOG, "[hyprfocus] Flash onWindowFocus start");
  IFocusAnimation::onWindowFocus(pWindow, pHandle);

  static auto *const flash_opacity =
      (Hyprlang::FLOAT const *)(getConfigValue(pHandle, "flash_opacity")
                                    ->getDataStaticPtr());
  Debug::log(LOG, "[hyprfocus] Flash opacity: {}", *flash_opacity);
  pWindow->m_fAlpha = *flash_opacity;
  pWindow->m_fAlpha.setConfig(&m_sFocusInAnimConfig);
  pWindow->m_fAlpha.setCallbackOnEnd([this, pWindow, pHandle](void *) {
    // Make sure we restore to the active window opacity
    pWindow->m_fAlpha =
        *(Hyprlang::FLOAT const *)(HyprlandAPI::getConfigValue(
                                       pHandle, "decoration:active_opacity")
                                       ->getDataStaticPtr());
    pWindow->m_fAlpha.setConfig(&m_sFocusOutAnimConfig);
  });
}
