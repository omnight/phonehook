#ifndef QUICKSAND_H
#define QUICKSAND_H

#include <QObject>

class QuickSandHeader : public QObject
{
    Q_OBJECT
public:
    explicit QuickSandHeader(QByteArray j, long long k, QObject *parent = 0);

    long long h0, h1, h2, h4, h5, h6, h7;
    QList<long long> h3;
    long long h8(QList<long long> j);
    QList<long long> h9(QString j);
    long long ha(long long k);
    long long hb(long long j, long long k);
    void hc();

    long long getSize() { return h0; }
    long long getHalfSize() { return h1; }
    long long sipNode(long long j, long long k) { return ha(2 * j + k) & h2; }
    QList<long long> sipEdge(long long j);

signals:

public slots:

};


class QuickSand : public QObject
{
    Q_OBJECT
public:
    explicit QuickSand(QObject *parent = 0);


    QString hashList(QList<long long> l);
    QList< QList<long long> > solve(int iterations, QString data, int n, int o, int p);
    QList<long long> solveIteration(QString l, long long m, long long n, long long o);
    QList<long long> recoverSolution(long long l, long long m, QList<long long> n, QList<long long> o, QuickSandHeader &p, long long q, long long r);
    long long path(long long l, QList<long long> &m, QList<long long> n, long long o);


signals:

public slots:

};

#endif // QUICKSAND_H
