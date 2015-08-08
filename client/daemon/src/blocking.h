#ifndef BLOCKING_H
#define BLOCKING_H

#include <QObject>
#include "phonenumber.h"

class blocking : public QObject
{
    Q_OBJECT
private:
    explicit blocking(QObject *parent = 0);
    static blocking *m_Instance;

public:   
    static blocking *Instance();

    bool preCheckBlock(phonenumber number);

    bool checkContactBlock(phonenumber number);
    bool checkManualBlock(phonenumber number);
    bool checkAutoBlock(phonenumber number);

    void hangup();

signals:

public slots:

};

#endif // BLOCKING_H
