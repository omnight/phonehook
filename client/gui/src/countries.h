#ifndef COUNTRIES_H
#define COUNTRIES_H

#include <QObject>
#include <QHash>

namespace countries {

    QString getCountryNameISO3166(const QString &code);
};

#endif // COUNTRIES_H
