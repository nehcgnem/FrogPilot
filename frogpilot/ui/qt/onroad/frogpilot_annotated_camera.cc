#include "frogpilot/ui/qt/onroad/frogpilot_annotated_camera.h"

FrogPilotAnnotatedCameraWidget::FrogPilotAnnotatedCameraWidget(QWidget *parent) : QWidget(parent) {
}

void FrogPilotAnnotatedCameraWidget::showEvent(QShowEvent *event) {
  UIState *s = uiState();
  UIScene &scene = s->scene;
  FrogPilotUIState *fs = frogpilotUIState();
  //FrogPilotUIScene &frogpilot_scene = fs->frogpilot_scene;
  QJsonObject &frogpilot_toggles = fs->frogpilot_toggles;

  if (scene.is_metric || frogpilot_toggles.value("use_si_metrics").toBool()) {
    accelerationUnit = tr("m/s²");
    leadDistanceUnit = tr("meters");
    leadSpeedUnit = frogpilot_toggles.value("use_si_metrics").toBool() ? tr("m/s") : tr("km/h");

    accelerationConversion = 1.0f;
    distanceConversion = 1.0f;
    speedConversion = scene.is_metric ? MS_TO_KPH : MS_TO_MPH;
    speedConversionMetrics = frogpilot_toggles.value("use_si_metrics").toBool() ? 1.0f : MS_TO_KPH;
  } else {
    accelerationUnit = tr("ft/s²");
    leadDistanceUnit = tr("feet");
    leadSpeedUnit = tr("mph");

    accelerationConversion = METER_TO_FOOT;
    distanceConversion = METER_TO_FOOT;
    speedConversion = MS_TO_MPH;
    speedConversionMetrics = MS_TO_MPH;
  }
}

void FrogPilotAnnotatedCameraWidget::updateState(const UIState &s, const FrogPilotUIState &fs, const QJsonObject &frogpilot_toggles) {
  const FrogPilotUIScene &frogpilot_scene = fs.frogpilot_scene;

  speedLimitChanged = frogpilot_scene.speed_limit_changed;

  speedLimitOffset = frogpilot_scene.speed_limit_offset * speedConversion;
  unconfirmedSpeedLimit = frogpilot_scene.unconfirmed_speed_limit * speedConversion;

  speedLimitOffsetStr = (speedLimitOffset == 0) ? "–" : QString::number(speedLimitOffset, 'f', 0).prepend((speedLimitOffset > 0) ? "+" : "-");

  update();
}

void FrogPilotAnnotatedCameraWidget::paintEvent(QPaintEvent *event) {
  static QElapsedTimer pendingLimitTimer;

  if (speedLimitChanged) {
    if (!pendingLimitTimer.isValid()) {
      pendingLimitTimer.start();
    }

    QString newSpeedLimitStr = (unconfirmedSpeedLimit > 1) ? QString::number(std::nearbyint(unconfirmedSpeedLimit)) : "–";

    newSpeedLimitRect = speedLimitRect.translated(speedLimitRect.width() + 25, 0);
    newSpeedLimitRect.setWidth(newSpeedLimitStr.size() >= 3 ? 200 : 175);

    QPainter p(this);
    p.setBrush(whiteColor());
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(newSpeedLimitRect, 24, 24);
    p.setPen(pendingLimitTimer.isValid() && pendingLimitTimer.elapsed() % 1000 <= 500 ? QPen(blackColor(), 6) : QPen(redColor(), 6));
    p.drawRoundedRect(newSpeedLimitRect.adjusted(9, 9, -9, -9), 16, 16);

    p.setFont(InterFont(28, QFont::DemiBold));
    p.drawText(newSpeedLimitRect.adjusted(0, 22, 0, 0), Qt::AlignTop | Qt::AlignHCenter, tr("PENDING"));
    p.drawText(newSpeedLimitRect.adjusted(0, 51, 0, 0), Qt::AlignTop | Qt::AlignHCenter, tr("LIMIT"));
    p.setFont(InterFont(70, QFont::Bold));
    p.drawText(newSpeedLimitRect.adjusted(0, 85, 0, 0), Qt::AlignTop | Qt::AlignHCenter, newSpeedLimitStr);
  } else {
    pendingLimitTimer.invalidate();
  }
}
