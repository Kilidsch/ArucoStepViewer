import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    Label {
        text: "Thresholding"
        width: parent.width
        Layout.alignment: Qt.AlignHCenter
    }

    Grid {
        width: parent.width
        columns: 2
        Label {
            text: "Min. win. size: "
        }
        SpinBox {
            id: thresh_win_min
            from: 3
            to: 255
            value: _aruco.thresh_win_min
        }
        Binding {
            target: _aruco
            property: "thresh_win_min"
            value: thresh_win_min.value
        }

        Label {
            text: "Max. win. size: "
        }
        SpinBox {
            id: thresh_win_max
            from: 4
            to: 255
            value: _aruco.thresh_win_max
        }
        Binding {
            target: _aruco
            property: "thresh_win_max"
            value: thresh_win_max.value
        }

        Label {
            text: "Size step: "
        }
        SpinBox {
            id: thresh_win_step
            from: 1
            to: 255
            value: _aruco.thresh_win_step
        }
        Binding {
            target: _aruco
            property: "thresh_win_step"
            value: thresh_win_step.value
        }


        Label {
            text: "Thresh. const."
        }
        DoubleSpinBox {
            id: thresh_const
            from: 0
            to: 999999
            value: _aruco.thresh_const * decimalFactor
        }
        Binding {
            target: _aruco
            property: "thresh_const"
            value: thresh_const.value / thresh_const.decimalFactor
        }
    }

    Label {
        text: "Filtering"
        Layout.alignment: Qt.AlignHCenter
    }

    Grid {
        width: parent.width
        columns: 2
        Label {
            text: "Min. perimeter: "
        }
        DoubleSpinBox {
            id: perimeter_min
            from: 0
            to: 255
            value: _aruco.minMarkerPerimeterRate * decimalFactor
        }
        Binding {
            target: _aruco
            property: "minMarkerPerimeterRate"
            value: perimeter_min.value / perimeter_min.decimalFactor
        }


        Label {
            text: "Max. perimeter: "
        }
        DoubleSpinBox {
            id: perimeter_max
            from: 1
            to: 99999
            value: _aruco.maxMarkerPerimeterRate * decimalFactor
        }
        Binding {
            target: _aruco
            property: "maxMarkerPerimeterRate"
            value: perimeter_max.value / perimeter_max.decimalFactor
        }

        Label {
            text: "Min. corner dist: "
        }
        DoubleSpinBox {
            id: min_corner_dist
            from: 0
            to: 99999
            value: _aruco.minCornerDistanceRate * decimalFactor
        }
        Binding {
            target: _aruco
            property: "minCornerDistanceRate"
            value: min_corner_dist.value / min_corner_dist.decimalFactor
        }

        Label {
            text: "Poly. approx. rate: "
        }
        DoubleSpinBox {
            id: poly_approx
            from: 0
            to: 99999
            value: _aruco.polygonalApproxAccuracyRate * decimalFactor
        }
        Binding {
            target: _aruco
            property: "polygonalApproxAccuracyRate"
            value: poly_approx.value / poly_approx.decimalFactor
        }

        Label {
            text: "Min. marker dist. rate: "
        }
        DoubleSpinBox {
            id: min_marker_dist
            from: 0
            to: 99999
            value: _aruco.minMarkerDistanceRate * decimalFactor
        }
        Binding {
            target: _aruco
            property: "minMarkerDistanceRate"
            value: min_marker_dist.value / min_marker_dist.decimalFactor
        }
    }
}
