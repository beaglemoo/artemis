#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QWindow>
#include <QQuickItem>
#include <QQuickView>

/**
 * @brief Manages the Virtual Controller overlay during streaming
 *
 * This class handles the display of the on-screen virtual gamepad
 * overlay that allows touch/mouse input to send controller commands.
 */
class VirtualControllerManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isVisible READ isVisible WRITE setVisible NOTIFY visibilityChanged)
    QML_ELEMENT

public:
    explicit VirtualControllerManager(QObject *parent = nullptr);
    ~VirtualControllerManager();

    // Property getters
    bool isVisible() const { return m_isVisible; }

    // Visibility control
    Q_INVOKABLE void setVisible(bool visible);
    Q_INVOKABLE void toggle();
    Q_INVOKABLE void show();
    Q_INVOKABLE void hide();

    // Window management
    void setWindow(QWindow *window);
    void setWindowGeometry(int x, int y, int width, int height);

    // Enable/disable based on settings
    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }

public slots:
    // Button input handlers - connected to QML signals
    void onButtonPressed(int button);
    void onButtonReleased(int button);
    void onStickMoved(int stick, qreal x, qreal y);

signals:
    void visibilityChanged();

private:
    void createOverlay();
    void updateOverlayGeometry();
    void sendControllerState();

    bool m_isVisible;
    bool m_enabled;
    QWindow *m_window;
    QQuickView *m_overlayView;
    QQuickItem *m_rootItem;

    // Window geometry
    int m_windowX, m_windowY, m_windowWidth, m_windowHeight;
    bool m_hasWindowGeometry;

    // Controller state
    short m_buttonFlags;
    unsigned char m_leftTrigger;
    unsigned char m_rightTrigger;
    short m_leftStickX;
    short m_leftStickY;
    short m_rightStickX;
    short m_rightStickY;
};
