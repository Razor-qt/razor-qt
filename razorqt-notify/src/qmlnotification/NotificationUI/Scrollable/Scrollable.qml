import Qt 4.7

Rectangle {
    // Make the children of this Item children of the Item inside Flickable
    default property alias children: scrollableContents.children

    property int scrollbarWidth: 7

    property bool hideScrollBarsWhenStopped: true

    property alias contentWidth: scrollable.contentWidth
    property alias contentHeight: scrollable.contentHeight
    property alias flickableDirection: scrollable.flickableDirection

    property alias moving: scrollable.moving
    property alias movingHorizontally: scrollable.movingHorizontally
    property alias movingVertically: scrollable.movingVertically
    property alias flicking: scrollable.flicking
    property alias flickingHorizontally: scrollable.flickingHorizontally
    property alias flickingVertically: scrollable.flickingVertically

    // Ensure that we don't draw over siblings of the Scrollable
    clip: true

    Flickable {
        id: scrollable
        anchors.fill: parent

        Item {
            id: scrollableContents
            anchors.fill: parent
        }
    }

    // Vertical scrollbar
    ScrollBar {
        flickable: scrollable
        vertical: true
        scrollbarWidth: parent.scrollbarWidth
        hideScrollBarsWhenStopped: parent.hideScrollBarsWhenStopped
    }

    // Horizontal scrollbar
    ScrollBar {
        flickable: scrollable
        vertical: false
        scrollbarWidth: parent.scrollbarWidth
        hideScrollBarsWhenStopped: parent.hideScrollBarsWhenStopped
    }
}
