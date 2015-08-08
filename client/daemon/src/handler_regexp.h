#ifndef HANDLER_REGEXP_H
#define HANDLER_REGEXP_H

#include <QObject>
#include "process_data.h"
#include <QRegularExpression>
#include <QDomElement>

class robot_base;

class handler_regexp : public QObject
{
    Q_OBJECT
public:
    explicit handler_regexp(robot_base *parent = 0);


    QList<process_data*> processRegex(const QDomElement &robotXml, QString regexStr, process_data *inputData);

private:
    robot_base *owner;


signals:

public slots:

};

#endif // HANDLER_REGEXP_H
