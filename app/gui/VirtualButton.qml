import QtQuick 2.9

// Virtual Controller Button
// A single pressable button for the virtual controller overlay
Rectangle {
    id: button
    radius: width / 2
    color: pressed ? pressedColor : normalColor
    border.color: "#808080"
    border.width: 2
    opacity: 0.85

    property string text: ""
    property color normalColor: "#404040"
    property color pressedColor: "#606060"
    property bool pressed: false

    Text {
        anchors.centerIn: parent
        text: button.text
        color: "white"
        font.pointSize: Math.max(8, button.width * 0.3)
        font.bold: true
    }

    MouseArea {
        anchors.fill: parent
        onPressed: button.pressed = true
        onReleased: button.pressed = false
        onCanceled: button.pressed = false
    }

    // Handle touch input
    MultiPointTouchArea {
        anchors.fill: parent
        touchPoints: [
            TouchPoint { id: touch1 }
        ]
        onPressed: button.pressed = true
        onReleased: button.pressed = false
        onCanceled: button.pressed = false
    }
}
