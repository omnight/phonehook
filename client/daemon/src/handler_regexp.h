#ifndef HANDLER_REGEXP_H
#define HANDLER_REGEXP_H

#include <QObject>
#include "process_data.h"
#include <QRegularExpression>
#include <QDomElement>

class handler_regexp : public QObject
{
    Q_OBJECT
public:
    explicit handler_regexp(QObject *parent = 0);


    QList<process_data*> processRegex(const QDomElement &robotXml, QString regexStr, process_data *inputData);

signals:

public slots:

};

#endif // HANDLER_REGEXP_H
