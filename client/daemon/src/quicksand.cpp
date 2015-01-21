#include "quicksand.h"
#include <QDebug>
#include <QCryptographicHash>

QuickSand::QuickSand(QObject *parent) :
    QObject(parent)
{
}

long long QuickSand::path(long long l, QList<long long> &m, QList<long long> n, long long o) {
    long long p = 0;

    for(p = 0; l; l =  l < n.length() ? n[l] : 0) {
        if(++p >= o) {
            while(p-- && (p >= m.length() || m[p] != 1));
            if(p < 0)
                qDebug() << "Maximum path length was exceeded";
            else
                qDebug() << "Illegal cycle has occured";

        }

        while(m.length() <= p) m << 0;
        m[p] = l;
    }
    return p;
}

typedef QPair<long long, long long> lpair;

QList<long long> QuickSand::recoverSolution(long long l,
                                 long long m,
                                 QList<long long> n,
                                 QList<long long> o,
                                 QuickSandHeader &p,
                                 long long q,
                                 long long r) {

    QList<long long> t;
    QSet<lpair> u;

    u.insert(lpair(n[0], o[0]));

    //qDebug() << lpair(n[0], o[0]);

    while(l--)
        u.insert(lpair(n[(l + 1) & ~1], n[(l | 1)]));
    while(m--)
        u.insert(lpair(o[m | 1], o[(m + 1) & ~1]));
    long long v = 0;
    for(long long w = 0; w < r; w++) {
        lpair x(1 + p.sipNode(w, 0), 1 + p.getHalfSize() + p.sipNode(w, 1));
        if(u.contains(x)) {
            while(t.length() <= v) t << 0;
            t[v++] = w;
            u.remove(x);
        }
    }

    return t;

}

QList<long long> QuickSand::solveIteration(QString l, long long m, long long n, long long o) {

    int p = 8192;

    QuickSandHeader q(l.toUtf8(), m);
    long long r = o * q.getSize() / 100;
    QList<long long> s, t, u;

    t << 0;
    u << 0;

    for(long long v = 0; v < r; v++) {
        QList<long long> w = q.sipEdge(v);
        long long x = w[0],
                  y = w[1];
        x += 1;
        y += 1 + q.getHalfSize();

        QVariant z, aa;

        if(x < s.length()) { z = s[x]; }
        if(y < s.length()) { aa = s[y]; }

        if(z == y || aa == x) continue;

        t[0] = x;
        u[0] = y;

        long long ba = path(z.toLongLong(),t,s,p),
                  ca = path(aa.toLongLong(), u, s, p);

        //qDebug() << ba << ca;

        if((ba < t.length() ? t[ba] : QVariant()) == (ca < u.length() ? u[ca] : QVariant())) {
            long long da = qMin(ba, ca);

            for( ba -= da, ca -= da ;
                 (ba < t.length() ? t[ba] : QVariant() ) != (ca < u.length() ? u[ca] : QVariant() );
                 ba++, ca++);
            long long ea = ba + ca + 1;
            if(ea == n)
                return recoverSolution(ba, ca, t, u, q, n, r);
            continue;
        }

        if(ba < ca) {
            while(ba--)
                s[t[ba + 1]] = t[ba];
            while(s.length() <=  y) s << 0;
            s[x] = y;
        } else {
            while(ca--)
                s[u[ca + 1]] = u[ca];
            while(s.length() <=  y) s << 0;
            s[y] = x;
        }

    }
    return QList<long long>();

}

QString QuickSand::hashList(QList<long long> l) {

    QString m;
    for(int i=0; i < l.length(); i++) {
        m += (i > 0 ? "-" : "") + QString::number(l[i]);
    }
    return QCryptographicHash::hash(m.toLocal8Bit(), QCryptographicHash::Sha256).toHex();
}


QList< QList<long long> > QuickSand::solve(int iterations, QString data, int n, int o, int p) {

    QList< QList<long long> > q;
    QString currentData = data;
    for(int s=0; s < iterations; s++) {
        q.append(solveIteration(currentData, n, o, p));
        // qDebug() << q[0];
        currentData = hashList(q[s]);
    }
    return q;
}

// ==================== HEADER

long long QuickSandHeader::ha(long long k) {

    h4 = h3[0];
    h5 = h3[1];
    h6 = h3[2];
    h7 = h3[3] ^ k;

    for(int l = 0; l < 2; l++)
       hc();

    h4 ^= k;
    h6 ^= 0xFF;

    for(int l = 0; l < 4; l++)
        hc();

    return h4 ^ h5 ^ h6 ^ h7;
}

// J = "fe240829c20d949da7751eaf7a6327f63cc708e7679ac9e7be8933cc49c65a90"
// OUT = [254, 36, 8, 41, 194, 13, 148, ...
// OK!
QList<long long> QuickSandHeader::h9(QString j) {
    QList<long long> k;
    for(int l = 0; l < j.length(); l+= 2) {
        k.append(QStringRef(&j, l, 2).toString().toUInt(0, 16));
    }
    return k;
}

// CHECK OK
QuickSandHeader::QuickSandHeader(QByteArray j, long long k, QObject *parent): QObject(parent) {
    h0 = 1 << k;
    h1 = h0 / 2;
    h2 = h1 - 1;
    h3 = QList<long long>();
    h4 = 0;
    h5 = 0;
    h6 = 0;
    h7 = 0;

    QString l = QCryptographicHash::hash(j, QCryptographicHash::Sha256).toHex();

    //qDebug() << l;

    long long m = h8(h9(l)),
              n = h8(h9(l).mid(8) );

    h3.append(m ^ QString("736f6d6570736575").toLongLong(0, 16));
    h3.append(n ^ QString("646f72616e646f6d").toLongLong(0, 16));
    h3.append(m ^ QString("6c7967656e657261").toLongLong(0, 16));
    h3.append(n ^ QString("7465646279746573").toLongLong(0, 16));

}

void QuickSandHeader::hc() {

    h4 += h5;
    h6 += h7;
    h5 = hb(h5, 13);
    h7 = hb(h7, 16);
    h5 = h5 ^ h4;
    h7 = h7 ^ h6;
    h4 = hb(h4, 32);
    h6 += h5;
    h4 += h7;
    h5 = hb(h5, 17);
    h7 = hb(h7, 21);
    h5 = h5 ^ h6;
    h7 = h7 ^ h4;
    h6 = hb(h6, 32);

}

long long QuickSandHeader::h8(QList<long long> j) {
    long long k = j[0],
              l = j[1] << 8,
              m = j[2] << 16,
              n = j[3] << 24,
              o = j[4] << 32,
              p = j[5] << 40,
              q = j[6] << 48,
              r = j[7] << 56;

    return k | l | m | n | o | p | q | r;
}


long long QuickSandHeader::hb(long long j, long long k) {
    return j << k | (unsigned long long)j >> (64 - k);
}


QList<long long> QuickSandHeader::sipEdge(long long j) {
    QList<long long> e;
    e.append( sipNode(j,0) );
    e.append( sipNode(j,1) );
    return e;
}
