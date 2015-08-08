#include "handler_format.h"
#include <QtXml>
#include "entities.h"
#include "quicksand.h"
#include "robot_base.h"
#include <QJSEngine>

handler_format::handler_format(robot_base *parent) :
    QObject(parent),
    owner(parent)
{
}


bool ishexnstring(const QString& string) {
  for (int i = 0; i < string.length(); i++) {
    if (isxdigit(string[i] == 0))
      return false;
  }
  return true;
}

//// http://qjson.sourceforge.net/docs/json__scanner_8cpp_source.html
/* This file is part of QJson
    2  *
    3  * Copyright (C) 2008 Flavio Castelli <flavio.castelli@gmail.com>
    4  *
    5  * This library is free software; you can redistribute it and/or
    6  * modify it under the terms of the GNU Lesser General Public
    7  * License version 2.1, as published by the Free Software Foundation.
    8  *
    9  *
   10  * This library is distributed in the hope that it will be useful,
   11  * but WITHOUT ANY WARRANTY; without even the implied warranty of
   12  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   13  * Lesser General Public License for more details.
   14  *
   15  * You should have received a copy of the GNU Lesser General Public License
   16  * along with this library; see the file COPYING.LIB.  If not, write to
   17  * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   18  * Boston, MA 02110-1301, USA.
   19  */
static QString unescape_json( const QByteArray& ba, bool* ok ) {

  *ok = false;
  QString res;
  QByteArray seg;
  bool bs = false;
  for ( int i = 0, size = ba.size(); i < size; ++i ) {
    const char ch = ba[i];
    if ( !bs ) {
      if ( ch == '\\' )
        bs = true;
      else
        seg += ch;
    } else {
      bs = false;
      switch ( ch ) {
        case 'b':
          seg += '\b';
          break;
        case 'f':
          seg += '\f';
          break;
        case 'n':
          seg += '\n';
          break;
        case 'r':
          seg += '\r';
          break;
        case 't':
          seg += '\t';
          break;
        case 'u':
        {
          res += QString::fromUtf8( seg );
          seg.clear();

          if ( i > size - 5 ) {
            //error
            return QString();
          }

          const QString hex_digit1 = QString::fromUtf8( ba.mid( i + 1, 2 ) );
          const QString hex_digit2 = QString::fromUtf8( ba.mid( i + 3, 2 ) );
          i += 4;

          if ( !ishexnstring( hex_digit1 ) || !ishexnstring( hex_digit2 ) ) {
            qCritical() << "Not an hex string:" << hex_digit1 << hex_digit2;
            return QString();
          }
          bool hexOk;
          const ushort hex_code1 = hex_digit1.toShort( &hexOk, 16 );
          if (!hexOk) {
            qCritical() << "error converting hex value to short:" << hex_digit1;
            return QString();
          }
          const ushort hex_code2 = hex_digit2.toShort( &hexOk, 16 );
          if (!hexOk) {
            qCritical() << "error converting hex value to short:" << hex_digit2;
            return QString();
          }

          res += QChar(hex_code2, hex_code1);
          break;
        }
        case '\\':
          seg  += '\\';
          break;
        default:
          seg += ch;
          break;
      }
    }
  }
  res += QString::fromUtf8( seg );
  *ok = true;
  return res;
}


QString qss_solve(QString parameters) {
    QRegularExpression rx("(\\d+),\"(.*?)\",(\\d+),(\\d+),(\\d+),\".*?\",\"(.*?)\"");
    QRegularExpressionMatch m = rx.match(parameters);

    if(m.hasMatch()) {

        QuickSand qss;
        bool ok;

        QList<QList<long long> > solution  =
        qss.solve(m.captured(1).toInt(),
                  unescape_json( m.captured(2).toLocal8Bit(), &ok ),
                  m.captured(3).toInt(),
                  m.captured(4).toInt(),
                  m.captured(5).toInt());

        QString sString = "[[";

        for(int i=0; i < solution.length(); i++) {
           if(i > 0) sString+=",";
           sString += "[";
               for(int j=0; j < solution[i].length(); j++) {
                   if(j > 0) sString += ",";
                   sString += QString::number(solution[i][j]);
               }
           sString += "]";
        }

        sString += "]";
        sString += ",\"" + m.captured(6) + "\"]";

        QByteArray resultBytes = sString.toLocal8Bit();
        return resultBytes.toBase64();
    }

    return "";
}

QString handler_format::percentUrl(QString url, QString encoding, PercentAction action) {

    QTextEncoder *enc = QTextCodec::codecForName(encoding.toLocal8Bit())->makeEncoder(QTextCodec::IgnoreHeader);
    QTextDecoder *dec = QTextCodec::codecForName(encoding.toLocal8Bit())->makeDecoder(QTextCodec::IgnoreHeader);

    QString resultStr;

    QRegularExpression encodeMatch("[^a-zA-Z0-9-._~]");
    QRegularExpression decodeMatch("(?:%[a-fA-F0-9]{2})+");

    QRegularExpressionMatchIterator i;

    if(action == PercentEncode)  i = encodeMatch.globalMatch(url);
    if(action == PercentDecode)  i = decodeMatch.globalMatch(url);

    int pos = 0;
    while(i.hasNext()) {
        QRegularExpressionMatch m = i.next();

        resultStr += url.mid(pos, m.capturedStart()-pos);

        if(action == PercentEncode) {
            QByteArray bytes = enc->fromUnicode( m.captured(0));
            foreach(uchar c, bytes) {
                resultStr += "%" + QString::number(c ,16);
            }
        }

        if(action == PercentDecode) {
            QByteArray bytes;
            for(int i=0; i < m.captured(0).length(); i += 3) {
                bytes.append(  m.captured().mid(i+1, 2).toInt(0, 16) );
            }
            resultStr += dec->toUnicode(bytes);
        }

        pos = m.capturedEnd();
    }

    resultStr += url.mid(pos, url.length()-pos);

    delete enc;
    delete dec;

    return resultStr;
}


void handler_format::format(QString method, QString params, process_data *p) {

    if(method.toLower() == "xml") {
        char buf[p->value.size() + 10];
        decode_html_entities_utf8(buf, p->value.toUtf8().data());
        p->value = buf;
    } else if(method.toLower() == "lowercase") {
            p->value = p->value.toLower();
    } else if(method.toLower() == "uppercase") {
            p->value = p->value.toUpper();
    } else if(method.toLower() == "urlencode") {
        // QUrl::toPercentEncoding only does UTF-8
        if(params == "") params = "utf-8";
        p->value = percentUrl(p->value, params, PercentEncode );
    } else if(method.toLower() == "urldecode") {
        if(params == "") params = "utf-8";
        p->value = percentUrl(p->value, params, PercentDecode );
    } else if(method.toLower() == "json") {
        bool ok = false;
        QString unJson = unescape_json(p->value.toUtf8(), &ok);
        if(ok) p->value = unJson;
    } else if(method.toLower() == "base64") {       // assume utf-8 encoding?
        p->value = QString::fromUtf8( QByteArray::fromBase64(p->value.toLocal8Bit()) );
    } else if(method.toLower() == "exe") {
        p->value = special_format(method.toLower(), p->value);
    } else if(method.toLower() == "formatstring") {     // .NET FormatString syntax
        QString formattedValue = params;
        QMap<QString,QString> map;
        for(int i=0; i < p->value_parts.length(); i++)
            map.insert( QString::number(i) , p->value_parts[i]);
        owner->expand_advanced(formattedValue, "\\{(\\d+)\\}", map);
        p->value = formattedValue;
    } else if(method.toLower() == "relativeurl") {

        process_data *up = p;
        while(up->url.isEmpty() && up->parent() != NULL) {
            up = (process_data*)up->parent();
        }

        //find base url...~~
        if(!up->url.isEmpty())
            p->value = QUrl(up->url).resolved(p->value).toString();
    } else if(method.toLower() == "regexmultireplace" || method.toLower() == "regexreplace") {
        // ignore here, handled by handler_regexp.cpp
    } else if(method.toLower() == "script") {
        p->value = script_handler(params, p);
    } else {
        qDebug() << "UNIMPLEMENTED FORMATTER: " << method << "(" << params << ")";
    }

}

QString handler_format::script_handler(QString script, process_data *p) {

    QJSEngine se;
    QJSValue f = se.evaluate("(" + script + ")");

    if(f.isCallable()) {
        QJSValueList args;
        args << p->value;
        QJSValue r = f.call(args);
        return r.toString();
    }

    return p->value;

}

QString handler_format::special_format(QString function, QString param) {

    if(function.endsWith("quicksand.exe"))
      return qss_solve(param);

    if(function.endsWith("lgnjs.exe")) {        // timestamp (seconds since 1970 UTC)
        return QString::number( QDateTime::currentMSecsSinceEpoch()/1000 );
    }

    if(function.endsWith("timezone.exe")) {     // negative utc offset (minutes)
        QTimeZone z(QTimeZone::systemTimeZoneId());
        return QString::number( -z.offsetFromUtc(QDateTime::currentDateTime()) / 60 );
    }

    qDebug() << "unimplemented special formatter:" << function;
    return param;
}

