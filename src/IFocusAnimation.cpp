#include "IFocusAnimation.hpp"
#include "globals.hpp"
#include "log.hpp"

#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprlang.hpp>

void IFocusAnimation::init(HANDLE pHandle, std::string animationName) {
  m_szAnimationName = animationName;
  hyprfocus_log(LOG, "Initializing focus animation: {}", animationName);

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

void IFocusAnimation::setup(HANDLE pHandle, std::string animationName) {
  hyprfocus_log(LOG, "Setting up focus animation: {}", animationName);
  static const auto *inBezier =
      (Hyprlang::STRING const *)(HyprlandAPI::getConfigValue(
                                     pHandle, configPrefix() + "in_bezier")
                                     ->getDataStaticPtr());
  static const auto *inSpeed =
      (Hyprlang::FLOAT *const *)(HyprlandAPI::getConfigValue(
                                     pHandle, configPrefix() + "in_speed")
                                     ->getDataStaticPtr());
  static const auto *outBezier =
      (Hyprlang::STRING const *)(HyprlandAPI::getConfigValue(
                                     pHandle, configPrefix() + "out_bezier")
                                     ->getDataStaticPtr());
  static const auto *outSpeed =
      (Hyprlang::FLOAT *const *)(HyprlandAPI::getConfigValue(
                                     pHandle, configPrefix() + "out_speed")
                                     ->getDataStaticPtr());
  g_sInBezier = *inBezier;
  g_fInSpeed = **inSpeed;
  g_sOutBezier = *outBezier;
  g_fOutSpeed = **outSpeed;
  hyprfocus_log(LOG,
                "In bezier: {}\nIn speed: {}\nOut bezier: {}\nOut speed: {}",
                g_sInBezier, g_fInSpeed, g_sOutBezier, g_fOutSpeed);
}

void IFocusAnimation::onWindowFocus(CWindow *pWindow, HANDLE pHandle) {
  hyprfocus_log(LOG, "Base callback for animation: {}", m_szAnimationName);
  m_sFocusInAnimConfig.internalBezier = g_sInBezier;
  m_sFocusInAnimConfig.internalSpeed = g_fInSpeed;

  m_sFocusOutAnimConfig.internalBezier = g_sOutBezier;
  m_sFocusOutAnimConfig.internalSpeed = g_fOutSpeed;

  hyprfocus_log(
      LOG, "In bezier: {}\nIn speed: {}\nOut bezier: {}\nOut speed: {}",
      m_sFocusInAnimConfig.internalBezier, m_sFocusInAnimConfig.internalSpeed,
      m_sFocusOutAnimConfig.internalBezier,
      m_sFocusOutAnimConfig.internalSpeed);
}

void IFocusAnimation::addConfigValue(HANDLE pHandle, std::string name,
                                     Hyprlang::CConfigValue sValue) {
  HyprlandAPI::addConfigValue(pHandle, configPrefix() + name, sValue);
  hyprfocus_log(LOG, "Added config value: {}", configPrefix() + name);
}

Hyprlang::CConfigValue *IFocusAnimation::getConfigValue(HANDLE pHandle,
                                                        std::string name) {
  hyprfocus_log(LOG, "Getting config value: {}", configPrefix() + name);
  return HyprlandAPI::getConfigValue(pHandle, configPrefix() + name);
}
