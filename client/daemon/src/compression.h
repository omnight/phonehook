#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <QObject>

class Compression : public QObject
{
    Q_OBJECT
public:
    explicit Compression(QObject *parent = 0);

    static QByteArray gUncompress(const QByteArray &data);

signals:

public slots:

};

#endif // COMPRESSION_H
