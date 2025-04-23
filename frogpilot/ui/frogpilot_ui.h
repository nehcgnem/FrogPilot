#pragma once

#include <iostream>
#include <memory>

#include <QObject>

#include "cereal/messaging/messaging.h"
#include "selfdrive/ui/qt/network/wifi_manager.h"

#include "frogpilot/ui/qt/widgets/frogpilot_controls.h"

struct FrogPilotUIScene {
  bool always_on_lateral_active;
  bool downloading_update;
  bool enabled;
  bool force_onroad;
  bool frogpilot_panel_active;
  bool map_open;
  bool online;
  bool parked;
  bool reverse;
  bool speed_limit_changed;
  bool speed_limit_overridden;
  bool traffic_mode_enabled;
  bool wake_up_screen;

  int conditional_status;
  int driver_camera_timer;
  int started_timer;
  int steering_angle;

  float speed_limit;
  float speed_limit_offset;
  float speed_limit_overridden_speed;
  float unconfirmed_speed_limit;

  QJsonObject frogpilot_toggles;
};

class FrogPilotUIState : public QObject {
  Q_OBJECT

public:
  explicit FrogPilotUIState(QObject *parent = nullptr);

  void update();

  std::unique_ptr<SubMaster> sm;

  FrogPilotUIScene frogpilot_scene;

  Params params_memory{"/dev/shm/params"};

  QJsonObject &frogpilot_toggles = frogpilot_scene.frogpilot_toggles;

  WifiManager *wifi;

signals:
  void reviewModel();
};

FrogPilotUIState *frogpilotUIState();
