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

                        // extra lgic and bookkeeping to stay at selected tab after
                        // recopmutation with new params/image
                        property int oldIndex: 0
                        property var model: image_list
                        onModelChanged: {
                            if(oldIndex >= contentChildren.length){
                                oldIndex = contentChildren.length - 1
                            }

                            if(oldIndex > -1){
                                currentIndex = oldIndex
                            }
                        }

                        Repeater {
                            model: image_list
                            delegate: TabButton {
                                text: index
                                Component.onDestruction: {
                                    sub_bar.oldIndex = sub_bar.currentIndex
                                }
                            }
                        }
                    }
                    ImageStack{
                        id: img_stack
                        index: sub_bar.currentIndex
                        imgList: image_list
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}
