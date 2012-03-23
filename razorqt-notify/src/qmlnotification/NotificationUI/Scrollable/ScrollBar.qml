import Qt 4.7

Rectangle {
    // The flickable to which the scrollbar is attached to, must be set
    property variant flickable

    // True for vertical ScrollBar, false for horizontal
    property bool vertical: true

    // If set to false, scrollbar is visible even when not scrolling
    property bool hideScrollBarsWhenStopped: true

    // Thickness of the scrollbar, in pixels
    property int scrollbarWidth: 7

    color: "black"
    radius: vertical ? width/2 : height/2

    function sbOpacity()
    {
        if (!hideScrollBarsWhenStopped) {
            return 0.5;
        }

        return (flickable.flicking || flickable.moving) ? (vertical ? (height >= parent.height ? 0 : 0.5) : (width >= parent.width ? 0 : 0.5)) : 0;
    }

    // Scrollbar appears automatically when content is bigger than the Flickable
    opacity: sbOpacity()

    // Calculate width/height and position based on the content size and position of
    // the Flickable
    width: vertical ? scrollbarWidth : flickable.visibleArea.widthRatio * parent.width
    height: vertical ? flickable.visibleArea.heightRatio * parent.height : scrollbarWidth
    x: vertical ? parent.width - width : flickable.visibleArea.xPosition * parent.width
    y: vertical ? flickable.visibleArea.yPosition * parent.height : parent.height - height

    // Animate scrollbar appearing/disappearing
    Behavior on opacity { NumberAnimation { duration: 200 }}
}
