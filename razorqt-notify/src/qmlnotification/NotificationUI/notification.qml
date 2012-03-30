import QtQuick 1.0

Component {
    id: notificationComponent
    Rectangle {
        id: rectangle1

        Image {
            id: image1
            x: 0
            y: 0
            width: 64
            height: 64
            sourceSize.height: 64
            sourceSize.width: 64
            source: "qrc:/qtquickplugin/images/template_image.png"
        }

        Text {
            id: text1
            x: 80
            y: 0
            width: 131
            height: 15
            text: qsTr("text")
            font.pixelSize: 12
        }
    }
}



