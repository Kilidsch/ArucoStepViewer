import QtQuick
import QtQuick.Controls

Item {
    id: item
    width: label.width + spinbox.width
    height: Math.max(label.height, spinbox.height)
    property string parameter
    property double param_value
    property string label
    property string tooltip
    property alias label_width: label.width

    Row{
        Label {
            id: label
            text: item.label
            ToolTip.delay: 1000
            ToolTip.visible: ma.containsMouse
            ToolTip.text: item.tooltip
            MouseArea {
                id: ma
                anchors.fill: parent
                hoverEnabled: true
            }
        }
        DoubleSpinBox {
            id: spinbox
            from: 1
            to: 99999
            value: item.param_value * decimalFactor
            ToolTip.visible: hovered
            ToolTip.delay: 1000
            ToolTip.text: item.tooltip
        }
        Binding {
            target: _aruco
            property: item.parameter
            value: spinbox.value / spinbox.decimalFactor
        }
    }
}
