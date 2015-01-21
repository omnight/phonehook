#ifndef ROBOT_BASE_H
#define ROBOT_BASE_H

#include <QObject>
#include <QtXml>
#include "process_data.h"
#include <QStack>
#include <QMap>
#include <QRegularExpression>
#include <QString>
#include "handler_url.h"
#include "handler_regexp.h"
#include "handler_format.h"

class robot_base : public QObject
{
    Q_OBJECT
public:
    explicit robot_base(QObject *parent = 0);

    QDomDocument robotXml;
    bool load(QString filename);
    bool parse(QString xml);

    QDomDocument run(QMap<QString, QString> parameters);

    static void expand(QString &expr);

private:

    handler_url handleUrl;
    handler_regexp handleRegex;
    handler_format handleFormat;

    static QMap<QString,QString> parameters;

    void processNode(QStack<process_data*> inputDataTree, const QDomElement &inputNode, QDomElement outputNode);

signals:

public slots:

};

#endif // ROBOT_BASE_H
