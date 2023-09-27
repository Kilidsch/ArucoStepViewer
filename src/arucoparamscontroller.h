#ifndef ARUCOPARAMSCONTROLLER_H
#define ARUCOPARAMSCONTROLLER_H

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

    cv::aruco::DetectorParameters getParams() const;

    double adaptiveThreshConstant() const;
    void setAdaptiveThreshConstant(double newVal);

    int adaptiveThreshWinSizeMax() const;
    void setAdaptiveThreshWinSizeMax(int newVal);

    int adaptiveThreshWinSizeMin() const;
    void setAdaptiveThreshWinSizeMin(int newVal);

    int adaptiveThreshWinSizeStep() const;
    void setAdaptiveThreshWinSizeStep(int newVal);

    double minCornerDistanceRate() const;
    void setMinCornerDistanceRate(double newVal);

    double minMarkerDistanceRate();
    void setMinMarkerDistanceRate(double newVal);

    double maxMarkerPerimeterRate() const;
    void setMaxMarkerPerimeterRate(double newVal);

    double minMarkerPerimeterRate() const;
    void setMinMarkerPerimeterRate(double newVal);

    int minDistanceToBorder() const;
    void setMinDistanceToBorder(int newVal);

    double polygonalApproxAccuracyRate() const;
    void setPolygonalApproxAccuracyRate(double newVal);

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
