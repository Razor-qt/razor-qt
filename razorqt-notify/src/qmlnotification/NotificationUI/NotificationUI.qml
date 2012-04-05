import QtQuick 1.0
import "Scrollable"
Item{
    id: mainItem
    width: 400
    height: 80
    opacity: 0.7
    function addSomeData( appName, appSummary, icon, body, id) {
        console.log("adding item with id=" + id)
        recipesModel.append({"applicationName" : appName, "applicationSummary" : appSummary,"applicationIcon" : icon, "applicationBody" : body, "notifId" : id })

        var currIndex = listView.count-1;
        listView.currentIndex = currIndex;
        if ( recipesModel.get(currIndex-1)){
            console.log("previous entry exists for item" + currIndex);
            recipesModel.get(currIndex).visableLeft = true
            recipesModel.get(currIndex-1).visableRight = true
        }
    }
    function removeItem(appName, appSummary, icon, body, id){
        var c = recipesModel.count
        for( var i=0;i<c;++i){
            if ( /*recipesModel.get(i).applicationName == appName &&
                    recipesModel.get(i).applicationSummary == appSummary &&*/
                    recipesModel.get(i).notifId == id /*&&
                    recipesModel.get(i).applicationBody == body*/){

                    recipesModel.remove(i)
            }
        }
    }
    function count(){
        return recipesModel.count
    }
    signal qmlRemoved(int id)

    Rectangle {
        id: mainView
        anchors.fill: parent
        color:"#0a000000"

        ListModel{
            id: recipesModel
        }

        Rectangle {
            id: page
            width: mainView.width; height: mainView.height
            color: "#0a000000"



            // Delegate for the recipes.  This delegate has two modes:
            // 1. List mode (default), which just shows the picture and title of the recipe.
            // 2. Details mode, which also shows the ingredients and method.
            Component {
                id: recipeDelegate

                Item {
                    id: recipe
                    property bool visableLeft : true
                    property bool visableRight : true

                    // Create a property to contain the visibility of the details.
                    // We can bind multiple element's opacity to this one property,
                    // rather than having a "PropertyChanges" line for each element we
                    // want to fade.

                    width: listView.width
                    height: mainItem.height
                    opacity: 1

                    // A simple rounded rectangle for the background
                    Rectangle {
                        id: background
                        x: 2; y: 2; width: parent.width - x*2; height: parent.height - y*2
                        color: "ivory"
//                        border.color: "orange"
                        radius: 5
                    }

                    // This mouse region covers the entire delegate.
                    // When clicked it changes mode to 'Details'.  If we are already
                    // in Details mode, then no change will happen.
                    MouseArea{
                        width: 20 ; height: 20
                        x:parent.width - 32
                        y:0
                        Image{
                            anchors.fill: parent
                            source: "pics/close.png"
                            width: 20
                            height: 20
                            rotation: 0
                        }
                        onVisibleChanged: { console.log("visible changed") }

                        onClicked: {

                            var c = recipesModel.count
                            for( var i=0;i<c;++i){
                                if ( recipesModel.get(i).notifId == notifId) {
                                    recipesModel.remove(i);
                                    break ;
                                }
                            }
                            mainItem.qmlRemoved(notifId);

                        }
                    }
                    MouseArea {
                        x: mainItem.width - 78
                        y: mainItem.height - 31
                        id: arrowLeft
                        width: 22
                        height: 22
                        visible: recipe.visableLeft
                        Image {
                            anchors.fill: parent

                            rotation: -90
                            source: "pics/arrow-up.png"

                        }
                        onClicked: {
                            console.log("left clicked")
                            listView.decrementCurrentIndex()
                        }
                    }
                    MouseArea {
                        x: mainItem.width - 59
                        y: mainItem.height - 32
                        width: 22
                        height: 22
                        id: arrowRight
                        visible: recipe.visableRight
                        Image {
                            anchors.fill: parent

                            rotation: -90
                            source: "pics/arrow-down.png"
                        }
                        onClicked: {
                            console.log("left clicked")
                            listView.incrementCurrentIndex()
                        }
                    }

                    Row {
                        id: topLayout
                        x: 10; y: 10; height: recipeImage.height; width: parent.width
                        spacing: 10

                        Image {
                            id: recipeImage
                            width: 64; height: 64
                            source: applicationIcon
                        }

                        Column {
                            width: background.width - recipeImage.width - 20; height: recipeImage.height
                            spacing: 2

                            Text {
                                id: applicationSummaryBox
                                text: applicationSummary
                                font.bold: true; font.pointSize: 12
                                wrapMode: Text.WordWrap
                                width: parent.width
                            }

                            Text {
                                text: applicationName
                                wrapMode: Text.WordWrap
                                width: parent.width
                            }
                            Text{
                                text:applicationBody
                                wrapMode: Text.WordWrap
                                width: parent.width
                                font.pointSize: 8
                            }
                        }
                    }

                    // A button to close the detailed view, i.e. set the state back to default ('').
                    ListView.onAdd: {
                        console.log(" on add: current index=" + listView.currentIndex);
                        listView.currentIndex = listView.count - 1
                    }
                    ListView.onRemove: SequentialAnimation {
                    }
                }
            }
            // The actual list
            ListView {
                id: listView
                anchors.fill: parent
                model: recipesModel
                delegate: recipeDelegate
                opacity: 1
                interactive: false
                orientation: "Horizontal"
                highlightMoveSpeed: 1000
            }
        }
    }
}
