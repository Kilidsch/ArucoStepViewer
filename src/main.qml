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

    RowLayout {
        anchors.top: bar.bottom
        anchors.bottom: parent.bottom
        width: parent.width

        ArucoDetectorParams {

        }

        StackLayout {
            id: mystack
            width: parent.width
            height: parent.height - bar.height
            y: bar.height
            currentIndex: bar.currentIndex
            Repeater {
                width: parent.width
                height: parent.height
                model: ImageModel
                delegate: ColumnLayout{
                    width: parent.width
                    height: parent.height
                    spacing: 0
                    TabBar {
                        id: sub_bar
                        width: parent.width
                        Repeater {
                            model: image_list
                            delegate: TabButton {
                                text: index
                            }
                        }
                    }
                    StackLayout
                    {
                        currentIndex: sub_bar.currentIndex
                        Repeater {
                            model: image_list
                            ImageItem {
                                img: image_list[index]
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }
        }
    }
}
