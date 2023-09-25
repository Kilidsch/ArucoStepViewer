#include "arucoparamscontroller.h"

ArucoParamsController::ArucoParamsController(QObject *parent) : QObject{parent}
{
    connect(this, &ArucoParamsController::adaptiveThreshConstantChanged, this, [this](){emit paramsChanged();});
    connect(this, &ArucoParamsController::adaptiveThreshWinSizeMaxChanged, this, [this](){emit paramsChanged();});
    connect(this, &ArucoParamsController::adaptiveThreshWinSizeMinChanged, this, [this](){emit paramsChanged();});
    connect(this, &ArucoParamsController::adaptiveThreshWinSizeStepChanged, this, [this](){emit paramsChanged();});
    connect(this, &ArucoParamsController::minCornerDistanceRateChanged, this, [this](){emit paramsChanged();});
    connect(this, &ArucoParamsController::minMarkerDistanceRateChanged, this, [this](){emit paramsChanged();});
    connect(this, &ArucoParamsController::maxMarkerPerimeterRateChanged, this, [this](){emit paramsChanged();});
    connect(this, &ArucoParamsController::minMarkerPerimeterRateChanged, this, [this](){emit paramsChanged();});
    connect(this, &ArucoParamsController::minDistanceToBorderChanged, this, [this](){emit paramsChanged();});
    connect(this, &ArucoParamsController::polygonalApproxAccuracyRateChanged, this, [this](){emit paramsChanged();});
}
