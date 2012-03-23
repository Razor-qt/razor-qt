import QtQuick 1.0

Component {
    id: notificationComponent
    Rectangle {
        id: rectangle1
        Image {
            id: icon
            y: 0
            width: 64
            height: 64
            anchors.left: parent.left
            anchors.leftMargin: 0
            source: "icon.png"
        }
        Text {
            id: appName
            x: 106
            y: 0
            text: qsTr("SomeAppName")
        }
        Text{
            id: appSummary
            x: 106
            y: 25
            text: qsTr("summary")
            anchors.top: icon.bottom
            anchors.topMargin: -39
        }
    }
}



