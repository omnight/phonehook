#include "overlay.h"
#include <qpa/qplatformnativeinterface.h>
#include "dbus_adapter.h"
#include <QColor>
#include <QQuickView>
#include <QQmlContext>
#include <QGuiApplication>

overlay::overlay(QObject *parent):QObject(parent)
{
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

    m_overlayView->setSource(SailfishApp::pathTo("qml/popup/gui.qml"));

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
