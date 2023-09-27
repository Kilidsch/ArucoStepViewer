import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    Label {
        text: "Thresholding"
        width: parent.width
        Layout.alignment: Qt.AlignHCenter
    }

    Column
    {
        id: thresh_col
        width: parent.width
        Layout.alignment: Qt.AlignHCenter
        Component.onCompleted: {
            var max = 0;
            for (var i = 0; i < thresh_col.children.length; ++i)
            {
                if (thresh_col.children[i].label_width > max) {
                    max = thresh_col.children[i].label_width
                }
            }
            for (var i = 0; i < thresh_col.children.length; ++i)
            {
                thresh_col.children[i].label_width = max;
            }
        }

        DoubleParameter {
            parameter: "thresh_const"
            param_value: _aruco.thresh_const
            label:  "Thresh. const."
            tooltip: "constant for adaptive thresholding before finding contours"
        }

        IntegerParameter {
            parameter: "thresh_win_max"
            param_value: _aruco.thresh_win_max
            label: "Max. win. size: "
            tooltip: "maximum window size for adaptive thresholding before finding contours"
        }

        IntegerParameter {
            parameter: "thresh_win_min"
            param_value: _aruco.thresh_win_min
            label: "Min. win. size: "
            tooltip: "minimum window size for adaptive thresholding before finding contours "
        }

        IntegerParameter {
            parameter: "thresh_win_step"
            param_value: _aruco.thresh_win_step
            label: "Size step: "
            tooltip: "increments from adaptiveThreshWinSizeMin to adaptiveThreshWinSizeMax during the thresholding "
        }

    }

    Label {
        text: "Filtering"
        Layout.alignment: Qt.AlignHCenter
    }

    Column
    {
        id: filter_col
        width: parent.width
        Layout.alignment: Qt.AlignHCenter
        Component.onCompleted: {
            var max = 0;
            for (var i = 0; i < filter_col.children.length; ++i)
            {
                if (filter_col.children[i].label_width > max) {
                    max = filter_col.children[i].label_width
                }
            }
            for (var i = 0; i < filter_col.children.length; ++i)
            {
                filter_col.children[i].label_width = max;
            }
        }

        DoubleParameter {
            parameter: "maxMarkerPerimeterRate"
            param_value: _aruco.maxMarkerPerimeterRate
            label: "Max. perimeter: "
            tooltip: "determine maximum perimeter for marker contour to be detected"
        }

        DoubleParameter {
            parameter: "minMarkerPerimeterRate"
            param_value: _aruco.minMarkerPerimeterRate
            label: "Min. perimeter: "
            tooltip: "determine minimum perimeter for marker contour to be detected"
        }

        DoubleParameter {
            parameter: "minCornerDistanceRate"
            param_value: _aruco.minCornerDistanceRate
            label: "Min. corner dist: "
            tooltip: "minimum distance between corners for detected markers relative to its perimeter"
        }

        DoubleParameter {
            parameter: "polygonalApproxAccuracyRate"
            param_value: _aruco.polygonalApproxAccuracyRate
            label: "Poly. approx. rate: "
            tooltip: "minimum accuracy during the polygonal approximation process to determine which contours are squares"
        }

        DoubleParameter {
            parameter: "minMarkerDistanceRate"
            param_value: _aruco.minMarkerDistanceRate
            label: "Min. marker dist. rate: "
            tooltip: "minimum mean distance beetween two marker corners to be considered imilar, so that the smaller one is removed"
        }
    }
}
