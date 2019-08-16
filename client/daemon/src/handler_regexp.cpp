#include <QDebug>

#include "handler_regexp.h"
#include "robot_base.h"

handler_regexp::handler_regexp(robot_base *parent) :
    QObject(parent),
    owner(parent)
{
}

/// regexStr expected format = /asdasd/
QList<process_data*> handler_regexp::processRegex(const QDomElement &robotXml, QString regexStr, process_data *inputData) {

    QQueue< QPair<QString,QVariant> > replaceActions;

    regexStr = QRegularExpression("^\\/(.*)\\/$", QRegularExpression::DotMatchesEverythingOption).match(regexStr).captured(1);

    // regex replace depending on decode attribute
    if(robotXml.attribute("decode", "").toLower() == "regexreplace") {
        replaceActions.enqueue( QPair<QString,QVariant>(regexStr ,robotXml.attribute("formatstring", "")));
    } else if(robotXml.attribute("decode", "").toLower() == "regexmultireplace") {
        QStringList regexes = regexStr.split(QRegularExpression("[\r\n]+"));
        QStringList replaces = robotXml.attribute("formatstring", "").split(QRegularExpression("[\r\n]+"));

        for(int i=0; i < regexes.count(); i++) {
            replaceActions.enqueue( QPair<QString,QVariant>(regexes[i], replaces[qMin(replaces.count()-1,i)]) );
        }
    } else {
        replaceActions.enqueue( QPair<QString,QVariant>(regexStr, QVariant()) );
    }

    QList<process_data*> results;

    if(inputData == nullptr) {
        qDebug() << "no input data!";
        return results;
    }

    qDebug() << "regex mapping" << replaceActions;

    QString inputString = inputData->value;

    for(QPair<QString,QVariant> &r: replaceActions) {

        owner->expand(r.first);
        QRegularExpression regex(r.first, QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);

        qDebug() << "processing regex" << r.first;

        int lastCaptureIndex = 0;
        int range=0;

        /// actually execute the regex

        QRegularExpressionMatchIterator i = regex.globalMatch(inputString);

        /// if replace -- create empty result (on first replace)
        if(!r.second.isNull()) {
            if(results.count() == 0) {
                results.append(new process_data);
                results.at(0)->node_id = robotXml.attribute("id");
            } else
                results.at(0)->value = "";
        }

        while(i.hasNext()) {
            QRegularExpressionMatch m = i.next();

            /// regex replace (1 result)
            if(!r.second.isNull()) {
                results.at(0)->value.append( inputString.mid( lastCaptureIndex, m.capturedStart() - lastCaptureIndex ) );

                QMap<QString,QString> replaceMap;
                for(int j=0; j < m.capturedTexts().length(); j++) {
                    replaceMap.insert(QString::number(j+1), m.capturedTexts().at(j));
                }

                QString res = r.second.toString();
                owner->expand_advanced(res, "\\$(\\d+)", replaceMap );
                results.at(0)->value.append( res );
                lastCaptureIndex = m.capturedEnd();
            /// regex match (multi results)
            } else {

                if(robotXml.hasAttribute("resultrange") && robotXml.attribute("resultrange") != QString::number(range)) {
                    range ++;
                    continue;
                }

                QStringList texts = m.capturedTexts();
                texts.removeFirst();

                if(texts.length() > 1)
                    qDebug() << "merge texts" << texts;

                process_data *pd = new process_data;
                pd->node_id = robotXml.attribute("id");
                pd->value_parts = texts;
                pd->value = texts.join("");

                results.append(pd);
            }

            range ++;
        }

        /// finish up replace operation
        if(!r.second.isNull()) {
            results.at(0)->value.append( inputString.mid( lastCaptureIndex, inputString.length() - lastCaptureIndex ) );
            inputString = results.at(0)->value;
        }

    };



    return results;
}
