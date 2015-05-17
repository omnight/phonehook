#ifndef SETTING_H
#define SETTING_H

#include <QObject>

class setting : public QObject
{
    Q_OBJECT
public:
    explicit setting(QObject *parent = 0);

    static QString get(QString key, QString defValue);

signals:

public slots:

};

#endif // SETTING_H
