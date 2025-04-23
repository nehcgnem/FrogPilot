#pragma once

#include "selfdrive/ui/qt/widgets/cameraview.h"

class FrogPilotAnnotatedCameraWidget : public QWidget {
  Q_OBJECT

public:
  explicit FrogPilotAnnotatedCameraWidget(QWidget *parent = 0);

  void updateState(const UIState &s, const FrogPilotUIState &fs, const QJsonObject &frogpilot_toggles);

  bool speedLimitChanged;
  bool viennaSpeedLimit;

  float speedLimitOffset;

  QRect newSpeedLimitRect;
  QRect speedLimitRect;

  QString speedLimitOffsetStr;

protected:
  void paintEvent(QPaintEvent *event) override;
  void showEvent(QShowEvent *event) override;

private:
  float accelerationConversion;
  float distanceConversion;
  float speedConversion;
  float speedConversionMetrics;
  float unconfirmedSpeedLimit;

  QColor blackColor(int alpha = 255) { return QColor(0, 0, 0, alpha); }
  QColor redColor(int alpha = 255) { return QColor(201, 34, 49, alpha); }
  QColor whiteColor(int alpha = 255) { return QColor(255, 255, 255, alpha); }

  QString accelerationUnit;
  QString leadDistanceUnit;
  QString leadSpeedUnit;
};
