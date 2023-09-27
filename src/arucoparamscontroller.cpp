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

cv::aruco::DetectorParameters ArucoParamsController::getParams() const
{
    return m_params;
}

double ArucoParamsController::adaptiveThreshConstant() const
{
    return m_params.adaptiveThreshConstant;
}

void ArucoParamsController::setAdaptiveThreshConstant(double newVal)
{
    if (m_params.adaptiveThreshConstant == newVal)
    {
        return;
    }
    m_params.adaptiveThreshConstant = newVal;
    emit adaptiveThreshConstantChanged();
}

int ArucoParamsController::adaptiveThreshWinSizeMax() const
{
    return m_params.adaptiveThreshWinSizeMax;
}

void ArucoParamsController::setAdaptiveThreshWinSizeMax(int newVal)
{
    if (m_params.adaptiveThreshWinSizeMax == newVal)
    {
        return;
    }
    m_params.adaptiveThreshWinSizeMax = newVal;
    emit adaptiveThreshWinSizeMaxChanged();
}

int ArucoParamsController::adaptiveThreshWinSizeMin() const
{
    return m_params.adaptiveThreshWinSizeMin;
}

void ArucoParamsController::setAdaptiveThreshWinSizeMin(int newVal)
{
    if (m_params.adaptiveThreshWinSizeMin == newVal)
    {
        return;
    }
    m_params.adaptiveThreshWinSizeMin = newVal;
    emit adaptiveThreshWinSizeMinChanged();
}

int ArucoParamsController::adaptiveThreshWinSizeStep() const
{
    return m_params.adaptiveThreshWinSizeStep;
}

void ArucoParamsController::setAdaptiveThreshWinSizeStep(int newVal)
{
    if (m_params.adaptiveThreshWinSizeStep == newVal)
    {
        return;
    }
    m_params.adaptiveThreshWinSizeStep = newVal;
    emit adaptiveThreshWinSizeStepChanged();
}

double ArucoParamsController::minCornerDistanceRate() const
{
    return m_params.minCornerDistanceRate;
}

void ArucoParamsController::setMinCornerDistanceRate(double newVal)
{
    if (m_params.minCornerDistanceRate == newVal)
    {
        return;
    }
    m_params.minCornerDistanceRate = newVal;
    emit minCornerDistanceRateChanged();
}

double ArucoParamsController::minMarkerDistanceRate()
{
    return m_params.minMarkerDistanceRate;
}

void ArucoParamsController::setMinMarkerDistanceRate(double newVal)
{
    if (m_params.minMarkerDistanceRate == newVal)
    {
        return;
    }

    m_params.minMarkerDistanceRate = newVal;
    emit minMarkerDistanceRateChanged();
}

double ArucoParamsController::maxMarkerPerimeterRate() const
{
    return m_params.maxMarkerPerimeterRate;
}

void ArucoParamsController::setMaxMarkerPerimeterRate(double newVal)
{
    if (m_params.maxMarkerPerimeterRate == newVal)
    {
        return;
    }
    m_params.maxMarkerPerimeterRate = newVal;
    emit maxMarkerPerimeterRateChanged();
}

double ArucoParamsController::minMarkerPerimeterRate() const
{
    return m_params.minMarkerPerimeterRate;
}

void ArucoParamsController::setMinMarkerPerimeterRate(double newVal)
{
    if (m_params.minMarkerPerimeterRate == newVal)
    {
        return;
    }
    m_params.minMarkerPerimeterRate = newVal;
    emit minMarkerPerimeterRateChanged();
}

int ArucoParamsController::minDistanceToBorder() const
{
    return m_params.minDistanceToBorder;
}

void ArucoParamsController::setMinDistanceToBorder(int newVal)
{
    if (m_params.minDistanceToBorder == newVal)
    {
        return;
    }
    m_params.minDistanceToBorder = newVal;
    emit minDistanceToBorderChanged();
}

double ArucoParamsController::polygonalApproxAccuracyRate() const
{
    return m_params.polygonalApproxAccuracyRate;
}

void ArucoParamsController::setPolygonalApproxAccuracyRate(double newVal)
{
    if (m_params.polygonalApproxAccuracyRate == newVal)
    {
        return;
    }
    m_params.polygonalApproxAccuracyRate = newVal;

    emit polygonalApproxAccuracyRateChanged();
}
