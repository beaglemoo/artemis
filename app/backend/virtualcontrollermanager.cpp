#include "virtualcontrollermanager.h"

#include <QQmlContext>
#include <QDebug>

// Limelight API for sending controller input
#include "Limelight.h"

VirtualControllerManager::VirtualControllerManager(QObject *parent)
    : QObject(parent)
    , m_isVisible(false)
    , m_enabled(false)
    , m_window(nullptr)
    , m_overlayView(nullptr)
    , m_rootItem(nullptr)
    , m_windowX(0)
    , m_windowY(0)
    , m_windowWidth(0)
    , m_windowHeight(0)
    , m_hasWindowGeometry(false)
    , m_buttonFlags(0)
    , m_leftTrigger(0)
    , m_rightTrigger(0)
    , m_leftStickX(0)
    , m_leftStickY(0)
    , m_rightStickX(0)
    , m_rightStickY(0)
{
}

VirtualControllerManager::~VirtualControllerManager()
{
    if (m_overlayView) {
        delete m_overlayView;
    }
}

void VirtualControllerManager::setEnabled(bool enabled)
{
    m_enabled = enabled;
    if (!enabled && m_isVisible) {
        hide();
    }
}

void VirtualControllerManager::setVisible(bool visible)
{
    if (!m_enabled && visible) {
        return; // Don't show if not enabled in settings
    }

    if (m_isVisible == visible) {
        return;
    }

    m_isVisible = visible;

    if (visible) {
        createOverlay();
    } else {
        if (m_overlayView) {
            m_overlayView->hide();
        }
    }

    emit visibilityChanged();
}

void VirtualControllerManager::toggle()
{
    setVisible(!m_isVisible);
}

void VirtualControllerManager::show()
{
    setVisible(true);
}

void VirtualControllerManager::hide()
{
    setVisible(false);
}

void VirtualControllerManager::setWindow(QWindow *window)
{
    m_window = window;
}

void VirtualControllerManager::setWindowGeometry(int x, int y, int width, int height)
{
    m_windowX = x;
    m_windowY = y;
    m_windowWidth = width;
    m_windowHeight = height;
    m_hasWindowGeometry = true;

    if (m_overlayView && m_isVisible) {
        updateOverlayGeometry();
    }
}

void VirtualControllerManager::createOverlay()
{
    qDebug() << "VirtualControllerManager::createOverlay() called";

    if (!m_overlayView) {
        qDebug() << "Creating new virtual controller overlay";
        m_overlayView = new QQuickView();
        m_overlayView->setResizeMode(QQuickView::SizeRootObjectToView);

        // Set up the QML context
        QQmlContext *context = m_overlayView->rootContext();
        if (!context) {
            qWarning() << "VirtualControllerManager: Failed to get QML context";
            delete m_overlayView;
            m_overlayView = nullptr;
            return;
        }
        context->setContextProperty("virtualControllerManager", this);

        // Load the QML file
        qDebug() << "Loading QML from: qrc:/gui/VirtualController.qml";
        m_overlayView->setSource(QUrl("qrc:/gui/VirtualController.qml"));

        if (m_overlayView->status() == QQuickView::Error) {
            qWarning() << "VirtualControllerManager: Error loading QML:" << m_overlayView->errors();
            delete m_overlayView;
            m_overlayView = nullptr;
            return;
        }

        m_rootItem = m_overlayView->rootObject();

        if (m_rootItem) {
            // Connect button signals
            connect(m_rootItem, SIGNAL(buttonPressed(int)),
                    this, SLOT(onButtonPressed(int)));
            connect(m_rootItem, SIGNAL(buttonReleased(int)),
                    this, SLOT(onButtonReleased(int)));
            connect(m_rootItem, SIGNAL(stickMoved(int, qreal, qreal)),
                    this, SLOT(onStickMoved(int, qreal, qreal)));

            // Set the controller to visible state
            m_rootItem->setProperty("showController", true);
            m_rootItem->setProperty("visible", true);

            qDebug() << "Virtual controller signals connected";
        }

        // Make it a popup overlay that stays on top
        m_overlayView->setFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);

        // Set transparent background
        m_overlayView->setColor(QColor(Qt::transparent));
    }

    if (m_overlayView) {
        updateOverlayGeometry();
        m_overlayView->show();
        m_overlayView->raise();
    }
}

void VirtualControllerManager::updateOverlayGeometry()
{
    if (!m_overlayView) {
        return;
    }

    if (m_hasWindowGeometry) {
        // Cover the full streaming window
        m_overlayView->setGeometry(m_windowX, m_windowY, m_windowWidth, m_windowHeight);
        qDebug() << "Virtual controller overlay geometry:" << m_windowX << m_windowY << m_windowWidth << m_windowHeight;
    } else {
        // Use a default size
        m_overlayView->setGeometry(0, 0, 1280, 720);
    }
}

void VirtualControllerManager::onButtonPressed(int button)
{
    qDebug() << "Virtual controller button pressed:" << Qt::hex << button;
    m_buttonFlags |= button;
    sendControllerState();
}

void VirtualControllerManager::onButtonReleased(int button)
{
    qDebug() << "Virtual controller button released:" << Qt::hex << button;
    m_buttonFlags &= ~button;
    sendControllerState();
}

void VirtualControllerManager::onStickMoved(int stick, qreal x, qreal y)
{
    // Validate stick index
    if (stick < 0 || stick > 1) {
        qWarning() << "Invalid stick index:" << stick;
        return;
    }

    // Clamp values to valid range before conversion
    x = qBound(-1.0, x, 1.0);
    y = qBound(-1.0, y, 1.0);

    // Convert -1.0 to 1.0 range to -32768 to 32767
    short stickX = (short)(x * 32767);
    short stickY = (short)(y * 32767);

    if (stick == 0) { // Left stick
        m_leftStickX = stickX;
        m_leftStickY = stickY;
    } else { // Right stick
        m_rightStickX = stickX;
        m_rightStickY = stickY;
    }

    sendControllerState();
}

void VirtualControllerManager::onTriggerMoved(int trigger, qreal value)
{
    // Clamp value to valid range before conversion
    value = qBound(0.0, value, 1.0);

    // Convert 0.0-1.0 range to 0-255
    unsigned char triggerValue = (unsigned char)(value * 255);

    if (trigger == 0) { // Left trigger
        m_leftTrigger = triggerValue;
    } else if (trigger == 1) { // Right trigger
        m_rightTrigger = triggerValue;
    } else {
        qWarning() << "Invalid trigger index:" << trigger;
        return;
    }

    sendControllerState();
}

void VirtualControllerManager::sendControllerState()
{
    // Send the controller state via Limelight API
    LiSendControllerEvent(
        m_buttonFlags,      // Button flags
        m_leftTrigger,      // Left trigger (0-255)
        m_rightTrigger,     // Right trigger (0-255)
        m_leftStickX,       // Left stick X
        m_leftStickY,       // Left stick Y
        m_rightStickX,      // Right stick X
        m_rightStickY       // Right stick Y
    );
}
