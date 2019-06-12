#include <qpa/qplatformnativeinterface.h>
#include <QColor>
#include <QQuickView>
#include <QQmlContext>
#include <QGuiApplication>
#include "overlay.h"
#include "dbus_adapter.h"
#include "setting.h"

overlay::overlay(QObject *parent):QObject(parent)
{
    showTimer.setSingleShot(true);
    showTimer.start(10000);
    connect(&showTimer, &QTimer::timeout, this, &overlay::onShowTimerTick);
}

void overlay::onShowTimerTick() {
    show();
}

void overlay::show() {

    m_overlayInitFinished = false;

    m_overlayView = SailfishApp::createView();

    // set backgrund to transparent
    QColor color;
    color.setRedF(0.0);
    color.setGreenF(0.0);
    color.setBlueF(0.0);
    color.setAlphaF(0.0);
    m_overlayView->setColor(color);
    m_overlayView->setClearBeforeRendering(true);

    m_overlayView->rootContext()->setContextProperty("_control", dbus_adapter::Instance());
    m_overlayView->rootContext()->setContextProperty("_view", this);

    if(setting::get("old_popup_design", "false") == "true") {
        m_overlayView->setSource(SailfishApp::pathTo("qml/popup/gui-old.qml"));
    } else {
        m_overlayView->setSource(SailfishApp::pathTo("qml/popup/gui.qml"));
    }

    m_overlayView->create();

    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    native->setWindowProperty(m_overlayView->handle(), QLatin1String("CATEGORY"), "notification");
    native->setWindowProperty(m_overlayView->handle(), QLatin1String("MOUSE_REGION"), QRegion(0, 0, 0, 0));

    m_overlayInitFinished = true;

    m_overlayView->showNormal();

}

void overlay::setClickArea(int x, int y, int w, int h) {

    if(m_overlayInitFinished) {
        QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
        native->setWindowProperty(m_overlayView->handle(), QLatin1String("MOUSE_REGION"), QRegion(x, y, w, h));
    }
}
