import QtQuick 2.9
import QtQuick.Controls 2.2

// Virtual Controller Overlay
// Provides on-screen gamepad buttons for touch/mouse input
Item {
    id: virtualController
    anchors.fill: parent
    visible: false
    opacity: 0.7

    property bool showController: false
    property real buttonSize: Math.min(parent.width, parent.height) * 0.08
    property real dpadSize: buttonSize * 2.5
    property real stickSize: buttonSize * 2

    signal buttonPressed(int button)
    signal buttonReleased(int button)
    signal stickMoved(int stick, real x, real y)

    // Button constants matching Limelight.h
    readonly property int BTN_A: 0x1000
    readonly property int BTN_B: 0x2000
    readonly property int BTN_X: 0x4000
    readonly property int BTN_Y: 0x8000
    readonly property int BTN_UP: 0x0001
    readonly property int BTN_DOWN: 0x0002
    readonly property int BTN_LEFT: 0x0004
    readonly property int BTN_RIGHT: 0x0008
    readonly property int BTN_START: 0x0010
    readonly property int BTN_BACK: 0x0020
    readonly property int BTN_LB: 0x0100
    readonly property int BTN_RB: 0x0200

    // Left side - D-Pad
    Item {
        id: dpadContainer
        width: dpadSize
        height: dpadSize
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 20

        // D-Pad Up
        VirtualButton {
            width: dpadSize / 3
            height: dpadSize / 3
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            text: "\u25B2"
            onPressedChanged: pressed ? buttonPressed(BTN_UP) : buttonReleased(BTN_UP)
        }

        // D-Pad Down
        VirtualButton {
            width: dpadSize / 3
            height: dpadSize / 3
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            text: "\u25BC"
            onPressedChanged: pressed ? buttonPressed(BTN_DOWN) : buttonReleased(BTN_DOWN)
        }

        // D-Pad Left
        VirtualButton {
            width: dpadSize / 3
            height: dpadSize / 3
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            text: "\u25C0"
            onPressedChanged: pressed ? buttonPressed(BTN_LEFT) : buttonReleased(BTN_LEFT)
        }

        // D-Pad Right
        VirtualButton {
            width: dpadSize / 3
            height: dpadSize / 3
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            text: "\u25B6"
            onPressedChanged: pressed ? buttonPressed(BTN_RIGHT) : buttonReleased(BTN_RIGHT)
        }
    }

    // Left shoulder button (LB)
    VirtualButton {
        width: buttonSize * 1.5
        height: buttonSize * 0.8
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 20
        text: "LB"
        onPressedChanged: pressed ? buttonPressed(BTN_LB) : buttonReleased(BTN_LB)
    }

    // Right side - Face buttons (A, B, X, Y)
    Item {
        id: faceButtonsContainer
        width: dpadSize
        height: dpadSize
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20

        // Y button (top)
        VirtualButton {
            width: buttonSize
            height: buttonSize
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            text: "Y"
            color: "#FFD700"
            onPressedChanged: pressed ? buttonPressed(BTN_Y) : buttonReleased(BTN_Y)
        }

        // A button (bottom)
        VirtualButton {
            width: buttonSize
            height: buttonSize
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            text: "A"
            color: "#00FF00"
            onPressedChanged: pressed ? buttonPressed(BTN_A) : buttonReleased(BTN_A)
        }

        // X button (left)
        VirtualButton {
            width: buttonSize
            height: buttonSize
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            text: "X"
            color: "#0080FF"
            onPressedChanged: pressed ? buttonPressed(BTN_X) : buttonReleased(BTN_X)
        }

        // B button (right)
        VirtualButton {
            width: buttonSize
            height: buttonSize
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            text: "B"
            color: "#FF0000"
            onPressedChanged: pressed ? buttonPressed(BTN_B) : buttonReleased(BTN_B)
        }
    }

    // Right shoulder button (RB)
    VirtualButton {
        width: buttonSize * 1.5
        height: buttonSize * 0.8
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 20
        text: "RB"
        onPressedChanged: pressed ? buttonPressed(BTN_RB) : buttonReleased(BTN_RB)
    }

    // Center buttons (Start and Back)
    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        spacing: 40

        VirtualButton {
            width: buttonSize * 1.2
            height: buttonSize * 0.7
            text: "Back"
            onPressedChanged: pressed ? buttonPressed(BTN_BACK) : buttonReleased(BTN_BACK)
        }

        VirtualButton {
            width: buttonSize * 1.2
            height: buttonSize * 0.7
            text: "Start"
            onPressedChanged: pressed ? buttonPressed(BTN_START) : buttonReleased(BTN_START)
        }
    }

    // Toggle button to show/hide controller
    Rectangle {
        id: toggleButton
        width: 60
        height: 30
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 10
        radius: 5
        color: "#80404040"
        border.color: "#808080"
        border.width: 1

        Text {
            anchors.centerIn: parent
            text: virtualController.visible ? "Hide" : "Show"
            color: "white"
            font.pointSize: 10
        }

        MouseArea {
            anchors.fill: parent
            onClicked: virtualController.visible = !virtualController.visible
        }
    }

    states: [
        State {
            name: "visible"
            when: showController
            PropertyChanges { target: virtualController; visible: true; opacity: 0.7 }
        },
        State {
            name: "hidden"
            when: !showController
            PropertyChanges { target: virtualController; visible: false; opacity: 0 }
        }
    ]

    transitions: [
        Transition {
            from: "hidden"; to: "visible"
            NumberAnimation { properties: "opacity"; duration: 200 }
        },
        Transition {
            from: "visible"; to: "hidden"
            NumberAnimation { properties: "opacity"; duration: 200 }
        }
    ]
}
