#ifndef PHONENUMBER_H
#define PHONENUMBER_H
#include <QObject>
#include <QList>

class phonenumber {

public:
    phonenumber() {}
    phonenumber(QString number, QString location, QString mnc);
    phonenumber(const phonenumber& other);

    QString number_local;
    QString country_prefix;
    QString number_international;
    QString country_iso;


    static QString mobilecc_to_iso32662(int code);
    static QString countryPrefix(QString country, QString prefix = "", QString number = "");
    static QString process(QString number);
    static QString convertNumber(QString number, QString numberCountryIso, QString locationIso);

    operator QMap<QString,QString>() const;

};

Q_DECLARE_METATYPE(phonenumber)

class rule {

public:

    explicit rule(const QString &country_name, const QString &country_prefix, const QString &exit_code, const QString &trunk_code, const QString &network_code = "");

    static const QList<rule> rules;
    static void initialize();

    static QList<rule>::const_iterator find(const QString &country_prefix, const QString &mnc);

    QString country_prefix;
    QString country_name;
    QString network_code;
    QString exit_code;
    QString trunk_code;
};



#endif // PHONENUMBER_H
