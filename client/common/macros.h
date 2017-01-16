#ifndef MACROS_H
#define MACROS_H

#include "util.h"
#include <QDebug>
#include <QSqlError>
#include <QVariantMap>
#include <QVariantList>

#define SINGLETON(CLASS)    \
    private: static CLASS *m_Instance; \
    public: static CLASS *Instance() { if(m_Instance==NULL) m_Instance = new CLASS(); return m_Instance; }

#define SINGLETON_CPP(CLASS)    \
    CLASS *CLASS::m_Instance = NULL;

#define CHECK_JNI_ERROR \
    QAndroidJniEnvironment env;\
    if (env->ExceptionCheck()) {\
        env->ExceptionDescribe(); \
        qFatal("ANDROID JNI EXCEPTION");\
    }

#define SQL_EXEC_CHECK(obj) \
    obj.exec(); \
    if(obj.lastError().isValid()) {\
        qCritical() << "SQL ERROR:" << obj.lastQuery() << obj.lastError().databaseText();\
    }


#define Q_PROP(type, name) Q_PROPERTY(type* name READ get_ ## name NOTIFY name ## _changed)

#define Q_PROP2(type, name) private: type m_ ## name; \
    type* get_ ## name() { return &m_ ## name; }\

#define Q_PROP_SIGNAL(type, name) void name ## _changed(type* data);


#define GET_MACRO(_1,_2,_3,_4,_5,_6,NAME,...) NAME
#define SQL(...) GET_MACRO(__VA_ARGS__, SQL6, SQL5, SQL4, SQL3, SQL2, SQL1)(__VA_ARGS__)

#define SQL1(a) util::queryToArray(a, QVariantMap())
#define SQL2(a,b) util::queryToArray(a, QVariantMap({ { #b, b}}))
#define SQL3(a,b,c) util::queryToArray(a, QVariantMap({ {#b, b}, {#c, c}}))
#define SQL4(a,b,c,d) util::queryToArray(a, QVariantMap({ {#b, b}, {#c, c},{#d, d}}))
#define SQL5(a,b,c,d,e) util::queryToArray(a, QVariantMap({ {#b, b}, {#c, c},{#d, d},{#e, e}}))
#define SQL6(a,b,c,d,e,f) util::queryToArray(a, QVariantMap({ {#b, b}, {#c, c},{#d, d},{#e, e},{#f, f}}))

#define SQL_FIRST(a) a.value<QVariantList>().at(0).value<QVariantMap>()
#define SQL_SCALAR(a) SQL_FIRST(a).values()[0]

#define toVM value<QVariantMap>()
#define toVL value<QVariantList>()

#endif // MACROS_H

