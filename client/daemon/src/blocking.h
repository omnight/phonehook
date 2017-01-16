#ifndef BLOCKING_H
#define BLOCKING_H

#include <QObject>
#include "phonenumber.h"
#include "macros.h"

class blocking : public QObject
{
    Q_OBJECT

    SINGLETON(blocking)

private:
    explicit blocking(QObject *parent = 0);

public:   
    bool preCheckBlock(phonenumber number);

    bool checkContactBlock(phonenumber number);
    bool checkManualBlock(phonenumber number);
    bool checkAutoBlock(phonenumber number);

    void hangup();

signals:

public slots:

};

#endif // BLOCKING_H
