#ifndef NORMALIZE_PHONENUMBER_H
#define NORMALIZE_PHONENUMBER_H

#include <QObject>

class normalize_phonenumber : public QObject
{
    Q_OBJECT
public:
    explicit normalize_phonenumber(QObject *parent = 0);


    static QString process(QString number);
    static QString international_to_local(QString number);

    static QString mobilecc_to_iso32662(int code);

    static QString countryPrefix(QString country);

signals:

public slots:

};

#endif // NORMALIZE_PHONENUMBER_H
