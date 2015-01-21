#include "robot_base.h"
#include "handler_url.h"
#include <QTextStream>

QMap<QString,QString> robot_base::parameters;

robot_base::robot_base(int botId, QObject *parent) :
    QObject(parent), handleUrl(botId) {
    this->botId = botId;
}

bool robot_base::parse(QString xml) {
    robotXml = QDomDocument("robot");
    return robotXml.setContent(xml);
}

bool robot_base::load(QString filename) {

    QFile f(filename);
    if(!f.open(QIODevice::ReadOnly)) {
        qDebug() << "COULD NOT OPEN FILE " << filename;
        return false;
    }

    robotXml = QDomDocument("robot");
    bool parseResult = robotXml.setContent(&f);
    f.close();

    qDebug() << "parsing xml contents " << parseResult;

    return parseResult;
}


void robot_base::processNode(QStack<process_data*> inputDataTree, const QDomElement &robotNode, QDomElement outputNode) {

    qDebug() << "processing node" << robotNode.attribute("name") << "output to" << outputNode.tagName();


    /// DETERMINE WHAT NODE TO PULL INPUT FROM

    process_data *inputData = NULL;
    if(robotNode.attribute("source", "") != "") {

        QString sourceNodeId = robotNode.attribute("source", "");
        qDebug() << "lookup data for node " << sourceNodeId;

        foreach(process_data *p, inputDataTree) {
            if(p->node_id == sourceNodeId)
                inputData = p;
        }

    } else if(inputDataTree.count() > 0) {
        inputData = inputDataTree.top();
    }

    if(inputData != NULL)
        qDebug() << "input data = " << inputData->node_id;

    QList<process_data*> output;

    /// SWITCH NODE TYPE

    if(robotNode.hasAttribute("value") || robotNode.hasAttribute("url")) {

        QString eval = "";
        if(robotNode.hasAttribute("value")) eval = robotNode.attribute("value");
        if(robotNode.hasAttribute("url")) eval = robotNode.attribute("url");

        if(eval.startsWith("/") && eval.endsWith("/")) {
            // process as regex
            output.append(handleRegex.processRegex(robotNode, eval, inputData));
        } else {
            // process as literal
            process_data *p = new process_data;
            p->node_id = robotNode.attribute("id");
            p->value =  eval;
            expand(p->value);

            output.append( p );
        }
    }

    // if this is an "inherit" type, append null value to execute children.
    if(!robotNode.hasAttribute("url") && !robotNode.hasAttribute("value")) {
        output.append(NULL);
    }

    /// PROCESS OUTPUT AND RECURSE
    foreach(process_data* p, output) {

        if(p && !p->parent())
            p->setParent(inputData);

        QDomElement newOutput = outputNode;

        /// perform decoding/formatting options
        if(robotNode.hasAttribute("decode"))
            handleFormat.format(robotNode.attribute("decode"), robotNode.attribute("formatstring"), p);

        /// download stuff from net
        if(robotNode.hasAttribute("url")) {
            handleUrl.loadUrl(robotNode, inputData, p);
        }

        /// trim
        if(p) p->value = p->value.trimmed();

        // create new xml node if necessary
        if(robotNode.hasAttribute("name") && !robotNode.attribute("name").startsWith("#")) {

            QString param = robotNode.attribute("paramoutput");

            if(param != "") {
                if(p)
                    parameters.insert(robotNode.attribute("name"), p->value);
            } else {
                QDomElement n = outputNode.ownerDocument().createElement( robotNode.attribute("name") );
                outputNode.appendChild(n);
                newOutput = n;

                if(p && robotNode.childNodes().count() == 0)
                    n.appendChild( outputNode.ownerDocument().createTextNode( p->value)  );
            }
        }


        /// process child nodes
        if(p) inputDataTree.push(p);

        for(int i=0; i < robotNode.childNodes().length(); i++) {
            QDomNode n = robotNode.childNodes().at(i);
            if(!n.isElement()) continue;
            processNode(inputDataTree, n.toElement(), newOutput);
        }

        if(p) delete inputDataTree.pop();

    }
}

QDomDocument robot_base::run(QMap<QString, QString> parameters) {
    robot_base::parameters = parameters;
    QStack<process_data*> inputData;

    QDomElement robot_root =
    robotXml.documentElement().elementsByTagName("set").at(0).toElement();

    QDomDocument output;
    QDomElement root = output.createElement("root");
    output.appendChild(root);

    processNode(inputData, robot_root, root);

    qDebug() << "========================================";
    qDebug() << "finished";

    output.replaceChild(root.firstChild(), root);

    if(root.hasChildNodes()) {
        output.replaceChild(root.firstChild(), root);
        //QTextStream ts(stdout);
        //root.firstChild().save(ts,2);
        //return robot_root.firstChild()
    }



    return output;
}


void robot_base::expand(QString &expr) {
    expand_advanced(expr, "\\$\\{([a-z0-9-_]+)\\}", parameters);
}

void robot_base::expand_advanced(QString &expr, QString regex, QMap<QString, QString> replacements) {

    QRegularExpression param_replace(regex, QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatchIterator i = param_replace.globalMatch(expr);

    QString newExpr = "";

    int lastCaptureIndex = 0;

    while(i.hasNext()) {
        QRegularExpressionMatch m = i.next();

        qDebug() << "attempt replace param " <<  m.captured(1);

        QString r;
        if(replacements.contains(m.captured(1))) {
            r = replacements[m.captured(1)];
            qDebug() << "replace value " << replacements[m.captured(1)];
        }

        newExpr.append( expr.mid( lastCaptureIndex, m.capturedStart() - lastCaptureIndex ) );
        newExpr.append(r);

        lastCaptureIndex = m.capturedEnd();

    }

    newExpr.append( expr.mid( lastCaptureIndex, expr.length() - lastCaptureIndex ) );

    expr = newExpr;

}

