import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform as Platform
import stepviewer

ApplicationWindow {
    id: root
    visible: true
    width: 640
    height: 480

    TabBar {
        id: bar
        width: parent.width
        Repeater {
            model: ImageModel
            delegate: TabButton {
                text: name
            }
        }
    }

    StackLayout {
        width: parent.width
        height: parent.height - bar.height
        y: bar.height
        currentIndex: bar.currentIndex
        Repeater {
            model: ImageModel
            delegate: ImageItem {
                img: image
                height: parent.height
                width: parent.width
            }
        }
    }
}
