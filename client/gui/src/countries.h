#ifndef COUNTRIES_H
#define COUNTRIES_H

#include <QObject>

class countries : public QObject
{
    Q_OBJECT
public:
    explicit countries(QObject *parent = 0);

    static QString getCountryNameISO3166(QString code);

signals:

public slots:

};

#endif // COUNTRIES_H
