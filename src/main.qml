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

                    onValueChanged: if(thresh_win_max.value <= value){
                                        thresh_win_max.value = value + 1
                                    }
                }

                Label {
                    text: "Max. win. size: "
                }
                SpinBox {
                    id: thresh_win_max
                    from: 4
                    to: 255

                    onValueChanged: if(thresh_win_min.value >= value){
                                        thresh_win_min.value = value - 1
                                    }
                }

                Label {
                    text: "Size step: "
                }
                SpinBox {
                    id: thresh_win_step
                    from: 1
                    to: 255
                }

                Label {
                    text: "Thresh. const."
                }
                SpinBox {
                    id: thresh_const
                    from: 0
                    to: 255
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
                SpinBox {
                    id: perimeter_min
                    from: 0
                    to: 255

                    onValueChanged: if(perimeter_max.value <= value){
                                        perimeter_max.value = value + 1
                                    }
                }

                Label {
                    text: "Max. perimeter: "
                }
                SpinBox {
                    id: perimeter_max
                    from: 1
                    to: 255

                    onValueChanged: if(perimeter_min.value >= value){
                                        perimeter_min.value = value - 1
                                    }
                }

                Label {
                    text: "Min. corner dist: "
                }
                DoubleSpinBox {
                    id: min_corner_dist
                    from: 0
                    to: 99999
                }

                Label {
                    text: "Poly. approx. rate: "
                }
                DoubleSpinBox {
                    id: poly_approx
                    from: 0
                    to: 99999
                }

                Label {
                    text: "Min. marker dist. rate: "
                }
                DoubleSpinBox {
                    id: min_marker_dist
                    from: 0
                    to: 99999
                }
            }
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
