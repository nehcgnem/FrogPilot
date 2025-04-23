#include "frogpilot/ui/frogpilot_ui.h"

static void update_state(FrogPilotUIState *fs) {
  FrogPilotUIScene &frogpilot_scene = fs->frogpilot_scene;

  SubMaster &sm = *(fs->sm);
  sm.update(0);

  if (sm.updated("carState")) {
    const cereal::CarState::Reader &carState = sm["carState"].getCarState();
    frogpilot_scene.parked = carState.getGearShifter() == cereal::CarState::GearShifter::PARK;
    frogpilot_scene.reverse = carState.getGearShifter() == cereal::CarState::GearShifter::REVERSE;
    frogpilot_scene.steering_angle = -carState.getSteeringAngleDeg();
  }
  if (sm.updated("controlsState")) {
    const cereal::ControlsState::Reader &controlsState = sm["controlsState"].getControlsState();
    frogpilot_scene.enabled = controlsState.getEnabled();
  }
  if (sm.updated("deviceState")) {
    const cereal::DeviceState::Reader &deviceState = sm["deviceState"].getDeviceState();
    frogpilot_scene.online = deviceState.getNetworkType() != cereal::DeviceState::NetworkType::NONE;
  }
  if (sm.updated("frogpilotCarState")) {
    const cereal::FrogPilotCarState::Reader &frogpilotCarState = sm["frogpilotCarState"].getFrogpilotCarState();
    frogpilot_scene.always_on_lateral_active = !frogpilot_scene.enabled && frogpilotCarState.getAlwaysOnLateralEnabled();
    frogpilot_scene.traffic_mode_enabled = frogpilotCarState.getTrafficModeEnabled();
  }
  if (sm.updated("frogpilotPlan")) {
    const cereal::FrogPilotPlan::Reader &frogpilotPlan = sm["frogpilotPlan"].getFrogpilotPlan();
    frogpilot_scene.speed_limit = frogpilotPlan.getSlcSpeedLimit();
    frogpilot_scene.speed_limit_changed = frogpilotPlan.getSpeedLimitChanged();
    frogpilot_scene.speed_limit_offset = frogpilotPlan.getSlcSpeedLimitOffset();
    frogpilot_scene.speed_limit_overridden = frogpilotPlan.getSlcOverridden();
    frogpilot_scene.speed_limit_overridden_speed = frogpilotPlan.getSlcOverriddenSpeed();
    frogpilot_scene.unconfirmed_speed_limit = frogpilotPlan.getUnconfirmedSlcSpeedLimit();
    if (frogpilotPlan.getTogglesUpdated()) {
      frogpilot_scene.frogpilot_toggles = QJsonDocument::fromJson(fs->params_memory.get("FrogPilotToggles").c_str()).object();
    }
  }
}

FrogPilotUIState::FrogPilotUIState(QObject *parent) : QObject(parent) {
  sm = std::make_unique<SubMaster, const std::initializer_list<const char *>>({
    "carState", "controlsState", "deviceState", "frogpilotCarState", "frogpilotDeviceState", "frogpilotPlan"
  });

  wifi = new WifiManager(this);

  frogpilot_scene.frogpilot_toggles = QJsonDocument::fromJson(QString::fromStdString(params_memory.get("FrogPilotToggles", true)).toUtf8()).object();
}

FrogPilotUIState *frogpilotUIState() {
  static FrogPilotUIState frogpilot_ui_state;
  return &frogpilot_ui_state;
}

void FrogPilotUIState::update() {
  update_state(this);

  frogpilot_scene.conditional_status = params_memory.getInt("CEStatus");
  frogpilot_scene.driver_camera_timer = frogpilot_toggles.value("driver_camera_in_reverse").toBool() && frogpilot_scene.reverse ? frogpilot_scene.driver_camera_timer + 1 : 0;
  frogpilot_scene.force_onroad = params_memory.getBool("ForceOnroad");
}
