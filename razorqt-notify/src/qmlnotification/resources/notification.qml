import QtQuick 1.0

Rectangle {
    id: rectangle1

    Rectangle {
        id: mainRect
        x: 0
        y: 0
        width: 354
        height: 64
        color: "#00000000"

        Image {
            id: appIcon
            x: 0
            y: 0
            width: 64
            height: 64
            source: "qrc:/close.png"
        }

        Text {
            id: appSummary
            x: 75
            y: 0
            width: 242
            height: 64
            text: qsTr("text")
            font.pixelSize: 12
        }

        MouseArea {
            id: mouse_area1
            x: 338
            y: 0
            width: 16
            height: 16

            Image {
                id: closeButton
                x: 0
                y: 0
                width: 16
                height: 16
                source: "qrc:/close.png"
            }
        }
    }
}



