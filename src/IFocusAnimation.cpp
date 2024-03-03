#include "IFocusAnimation.hpp"

#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprlang.hpp>

void IFocusAnimation::init(HANDLE pHandle, std::string animationName) {
  m_szAnimationName = animationName;
  Debug::log(LOG, "[hyprfocus] Initializing focus animation: {}",
             animationName);

  HyprlandAPI::addConfigValue(pHandle, configPrefix() + "in_bezier",
                              Hyprlang::STRING{"default"});
  HyprlandAPI::addConfigValue(pHandle, configPrefix() + "out_bezier",
                              Hyprlang::STRING{"default"});

  HyprlandAPI::addConfigValue(pHandle, configPrefix() + "in_speed",
                              Hyprlang::FLOAT{1.f});
  HyprlandAPI::addConfigValue(pHandle, configPrefix() + "out_speed",
                              Hyprlang::FLOAT{5.f});

  m_sFocusInAnimConfig =
      *(g_pConfigManager->getAnimationPropertyConfig("global"));
  m_sFocusInAnimConfig.internalEnabled = 1;
  m_sFocusInAnimConfig.internalStyle =
      std::string("hyprfocus_") + animationName + std::string("_in");
  m_sFocusInAnimConfig.pValues = &m_sFocusInAnimConfig;

  m_sFocusOutAnimConfig =
      *(g_pConfigManager->getAnimationPropertyConfig("global"));
  m_sFocusOutAnimConfig.internalEnabled = 1;
  m_sFocusOutAnimConfig.internalStyle =
      std::string("hyprfocus_") + animationName + std::string("_out");
  m_sFocusOutAnimConfig.pValues = &m_sFocusOutAnimConfig;
}

void IFocusAnimation::onWindowFocus(CWindow *pWindow, HANDLE pHandle) {
  Debug::log(LOG, "[hyprfocus] Base callback for animation: {}",
             m_szAnimationName);
  m_sFocusInAnimConfig.internalBezier =
      *(Hyprlang::STRING const *)(HyprlandAPI::getConfigValue(
                                      pHandle, configPrefix() + "in_bezier")
                                      ->getDataStaticPtr());
  m_sFocusInAnimConfig.internalSpeed =
      *(Hyprlang::FLOAT const *)(HyprlandAPI::getConfigValue(
                                     pHandle, configPrefix() + "in_speed")
                                     ->getDataStaticPtr());

  m_sFocusOutAnimConfig.internalBezier =
      *(Hyprlang::STRING const *)(HyprlandAPI::getConfigValue(
                                      pHandle, configPrefix() + "out_bezier")
                                      ->getDataStaticPtr());
  m_sFocusOutAnimConfig.internalSpeed =
      *(Hyprlang::FLOAT const *)(HyprlandAPI::getConfigValue(
                                     pHandle, configPrefix() + "out_speed")
                                     ->getDataStaticPtr());
  Debug::log(LOG, "[hyprfocus] Focus in bezier: {}",
             m_sFocusInAnimConfig.internalBezier);
  Debug::log(LOG, "[hyprfocus] Focus in speed: {}",
             m_sFocusInAnimConfig.internalSpeed);
  Debug::log(LOG, "[hyprfocus] Focus out bezier: {}",
             m_sFocusOutAnimConfig.internalBezier);
  Debug::log(LOG, "[hyprfocus] Focus out speed: {}",
             m_sFocusOutAnimConfig.internalSpeed);
}

void IFocusAnimation::addConfigValue(HANDLE pHandle, std::string name,
                                     Hyprlang::CConfigValue sValue) {
  HyprlandAPI::addConfigValue(pHandle, configPrefix() + name, sValue);
  Debug::log(LOG, "[hyprfocus] Added config value: {}", configPrefix() + name);
}

Hyprlang::CConfigValue *IFocusAnimation::getConfigValue(HANDLE pHandle,
                                                        std::string name) {
  Debug::log(LOG, "[hyprfocus] Getting config value: {}",
             configPrefix() + name);
  return HyprlandAPI::getConfigValue(pHandle, configPrefix() + name);
}
