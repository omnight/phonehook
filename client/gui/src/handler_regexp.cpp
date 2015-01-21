#include "handler_regexp.h"
#include <QDebug>
#include "robot_base.h"

handler_regexp::handler_regexp(QObject *parent) :
    QObject(parent)
{
}

/// regexStr expected format = /asdasd/
QList<process_data*> handler_regexp::processRegex(const QDomElement &robotXml, QString regexStr, process_data *inputData) {

    regexStr = QRegularExpression("^\\/(.*)\\/$", QRegularExpression::MultilineOption).match(regexStr).captured(1);
    robot_base::expand(regexStr);
    QRegularExpression regex(regexStr, QRegularExpression::DotMatchesEverythingOption);

    qDebug() << "processing regex" << regexStr;

    QList<process_data*> results;

    if(inputData == NULL) {
        qDebug() << "no input data!";
        return results;
    }

    int lastCaptureIndex = 0;
    int range=0;

    /// actually execute the regex

    QRegularExpressionMatchIterator i = regex.globalMatch(inputData->value);

    /// if replace -- create empty result
    if(robotXml.attribute("decode", "").toLower() == "regexreplace") {
        results.append(new process_data);
        results.at(0)->node_id = robotXml.attribute("id");
    }

    while(i.hasNext()) {
        QRegularExpressionMatch m = i.next();

        /// regex replace (1 result)
        if(robotXml.attribute("decode", "").toLower() == "regexreplace") {
            results.at(0)->value.append( inputData->value.mid( lastCaptureIndex, m.capturedStart() - lastCaptureIndex ) );
            results.at(0)->value.append( robotXml.attribute("formatstring", "") );
            lastCaptureIndex = m.capturedEnd();
        /// regex match (multi results)
        } else {

            if(robotXml.hasAttribute("resultrange") && robotXml.attribute("resultrange") != QString::number(range)) {
                range ++;
                continue;
            }

            QStringList texts = m.capturedTexts();
            texts.removeFirst();

            process_data *pd = new process_data;
            pd->node_id = robotXml.attribute("id");
            pd->value = texts.join("");

            results.append(pd);
        }

        range ++;
    }

    /// finish up replace operation
    if(robotXml.attribute("decode", "").toLower() == "regexreplace") {
        results.at(0)->value.append( inputData->value.mid( lastCaptureIndex, inputData->value.length() - lastCaptureIndex ) );
    }

    return results;
}
