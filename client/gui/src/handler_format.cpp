#include "handler_format.h"
#include <QtXml>
#include "entities.h"

handler_format::handler_format(QObject *parent) :
    QObject(parent)
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


void handler_format::format(QString method, QString params, process_data *p) {

    if(method.toLower() == "xml") {
        char buf[p->value.size() + 10];
        decode_html_entities_utf8(buf, p->value.toLocal8Bit().data());
        p->value = buf;
    } else if(method.toLower() == "urlencode") {
        p->value = QUrl::toPercentEncoding( p->value );
    } else if(method.toLower() == "urldecode") {
        p->value = QUrl::fromPercentEncoding( p->value.toUtf8() );
    } else if(method.toLower() == "json") {
        bool ok = false;
        QString unJson = unescape_json(p->value.toUtf8(), &ok);
        if(ok) p->value = unJson;
    } else {
        qDebug() << "UNIMPLEMENTED FORMATTER: " << method << "(" << params << ")";
    }

}



