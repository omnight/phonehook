#ifndef OVERLAY_H
#define OVERLAY_H

#include "sailfishapp.h"
#include <QObject>
#include <QTimer>

class overlay: public QObject
{
    Q_OBJECT

private:
    QQuickView *m_overlayView;
    bool m_overlayInitFinished;
    QTimer showTimer;

public:
    explicit overlay(QObject *parent = 0);
    Q_INVOKABLE void setClickArea(int, int, int, int);
    void show();

private slots:

    void onShowTimerTick();


};

#endif // OVERLAY_H
