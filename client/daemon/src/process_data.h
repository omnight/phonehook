#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H

#include <QObject>
#include <QNetworkCookie>
#include <QUrl>


class process_data : public QObject
{
    Q_OBJECT
public:
    explicit process_data(QObject *parent = 0);

    QString node_id;
    QString value;
    QList<QString> value_parts;
    QUrl url;

signals:

public slots:

};

#endif // PROCESS_DATA_H
