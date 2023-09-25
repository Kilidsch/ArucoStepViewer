#ifndef ARUCOPARAMSCONTROLLER_H
#define ARUCOPARAMSCONTROLLER_H

#include <QDebug>
#include <QObject>
#include <opencv2/aruco.hpp>

class ArucoParamsController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double thresh_const READ adaptiveThreshConstant WRITE setAdaptiveThreshConstant NOTIFY
                   adaptiveThreshConstantChanged FINAL)
    Q_PROPERTY(int thresh_win_max READ adaptiveThreshWinSizeMax WRITE setAdaptiveThreshWinSizeMax NOTIFY
                   adaptiveThreshWinSizeMaxChanged FINAL)
    Q_PROPERTY(int thresh_win_min READ adaptiveThreshWinSizeMin WRITE setAdaptiveThreshWinSizeMin NOTIFY
                   adaptiveThreshWinSizeMinChanged FINAL)
    Q_PROPERTY(int thresh_win_step READ adaptiveThreshWinSizeStep WRITE setAdaptiveThreshWinSizeStep NOTIFY
                   adaptiveThreshWinSizeStepChanged FINAL)
    Q_PROPERTY(double minCornerDistanceRate READ minCornerDistanceRate WRITE setMinCornerDistanceRate NOTIFY
                   minCornerDistanceRateChanged FINAL)
    Q_PROPERTY(double maxMarkerPerimeterRate READ maxMarkerPerimeterRate WRITE setMaxMarkerPerimeterRate NOTIFY
                   maxMarkerPerimeterRateChanged FINAL)
    Q_PROPERTY(double minMarkerPerimeterRate READ minMarkerPerimeterRate WRITE setMinMarkerPerimeterRate NOTIFY
                   minMarkerPerimeterRateChanged FINAL)
    Q_PROPERTY(int minDistanceToBorder READ minDistanceToBorder WRITE setMinDistanceToBorder NOTIFY
                   minDistanceToBorderChanged FINAL)
    Q_PROPERTY(double polygonalApproxAccuracyRate READ polygonalApproxAccuracyRate WRITE setPolygonalApproxAccuracyRate
                   NOTIFY polygonalApproxAccuracyRateChanged FINAL)
    Q_PROPERTY(double minMarkerDistanceRate READ minMarkerDistanceRate WRITE setMinMarkerDistanceRate NOTIFY
                   minMarkerDistanceRateChanged FINAL)
  public:
    explicit ArucoParamsController(QObject *parent = nullptr);

    cv::aruco::DetectorParameters getParams() const
    {
        return m_params;
    }

    double adaptiveThreshConstant() const
    {
        return m_params.adaptiveThreshConstant;
    }

    void setAdaptiveThreshConstant(double newVal)
    {
        if (m_params.adaptiveThreshConstant == newVal)
        {
            return;
        }
        m_params.adaptiveThreshConstant = newVal;
        emit adaptiveThreshConstantChanged();
    }

    int adaptiveThreshWinSizeMax() const
    {
        return m_params.adaptiveThreshWinSizeMax;
    }

    void setAdaptiveThreshWinSizeMax(int newVal)
    {
        if (m_params.adaptiveThreshWinSizeMax == newVal)
        {
            return;
        }
        m_params.adaptiveThreshWinSizeMax = newVal;
        emit adaptiveThreshWinSizeMaxChanged();
    }

    int adaptiveThreshWinSizeMin() const
    {
        return m_params.adaptiveThreshWinSizeMin;
    }

    void setAdaptiveThreshWinSizeMin(int newVal)
    {
        if (m_params.adaptiveThreshWinSizeMin == newVal)
        {
            return;
        }
        m_params.adaptiveThreshWinSizeMin = newVal;
        emit adaptiveThreshWinSizeMinChanged();
    }

    int adaptiveThreshWinSizeStep() const
    {
        return m_params.adaptiveThreshWinSizeStep;
    }

    void setAdaptiveThreshWinSizeStep(int newVal)
    {
        if (m_params.adaptiveThreshWinSizeStep == newVal)
        {
            return;
        }
        m_params.adaptiveThreshWinSizeStep = newVal;
        emit adaptiveThreshWinSizeStepChanged();
    }

    double minCornerDistanceRate() const
    {
        return m_params.minCornerDistanceRate;
    }

    void setMinCornerDistanceRate(double newVal)
    {
        if (m_params.minCornerDistanceRate == newVal)
        {
            return;
        }
        m_params.minCornerDistanceRate = newVal;
        emit minCornerDistanceRateChanged();
    }

    double minMarkerDistanceRate()
    {
        return m_params.minMarkerDistanceRate;
    }

    void setMinMarkerDistanceRate(double newVal)
    {
        if (m_params.minMarkerDistanceRate == newVal)
        {
            return;
        }

        m_params.minMarkerDistanceRate = newVal;
        emit minMarkerDistanceRateChanged();
    }

    double maxMarkerPerimeterRate() const
    {
        return m_params.maxMarkerPerimeterRate;
    }

    void setMaxMarkerPerimeterRate(double newVal)
    {
        if (m_params.maxMarkerPerimeterRate == newVal)
        {
            return;
        }
        m_params.maxMarkerPerimeterRate = newVal;
        emit maxMarkerPerimeterRateChanged();
    }

    double minMarkerPerimeterRate() const
    {
        return m_params.minMarkerPerimeterRate;
    }

    void setMinMarkerPerimeterRate(double newVal)
    {
        if (m_params.minMarkerPerimeterRate == newVal)
        {
            return;
        }
        m_params.minMarkerPerimeterRate = newVal;
        emit minMarkerPerimeterRateChanged();
    }

    int minDistanceToBorder() const
    {
        return m_params.minDistanceToBorder;
    }

    void setMinDistanceToBorder(int newVal)
    {
        if (m_params.minDistanceToBorder == newVal)
        {
            return;
        }
        m_params.minDistanceToBorder = newVal;
        emit minDistanceToBorderChanged();
    }

    double polygonalApproxAccuracyRate() const
    {
        return m_params.polygonalApproxAccuracyRate;
    }

    void setPolygonalApproxAccuracyRate(double newVal)
    {
        if (m_params.polygonalApproxAccuracyRate == newVal)
        {
            return;
        }
        m_params.polygonalApproxAccuracyRate = newVal;
        qDebug() << "New value plymoly " << newVal;

        emit polygonalApproxAccuracyRateChanged();
    }

  signals:
    void adaptiveThreshConstantChanged();
    void adaptiveThreshWinSizeMaxChanged();
    void adaptiveThreshWinSizeMinChanged();
    void adaptiveThreshWinSizeStepChanged();
    void minCornerDistanceRateChanged();
    void minMarkerDistanceRateChanged();
    void maxMarkerPerimeterRateChanged();
    void minMarkerPerimeterRateChanged();
    void minDistanceToBorderChanged();
    void polygonalApproxAccuracyRateChanged();

    void paramsChanged();

  private:
    cv::aruco::DetectorParameters m_params;
};

#endif // ARUCOPARAMSCONTROLLER_H
