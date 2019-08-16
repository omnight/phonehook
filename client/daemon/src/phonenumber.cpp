#include <QDebug>
#include <QRegularExpression>

#include "phonenumber.h"


phonenumber::phonenumber(QString number, QString location, QString mnc) {

    number = number.replace(QRegularExpression("[^+0-9]"), "");

    QString location_prefix = countryPrefix(location);

    qDebug() << "location prefix" << location_prefix;

    QList<rule>::const_iterator rule_found = rule::find(location_prefix, mnc);



    if(number.startsWith("+") || ( number.startsWith(rule_found->exit_code))) {
        qDebug() << "this is international number!";

        this->number_international = number;

        if(rule_found == rule::rules.end()){
            qDebug() << "no rule found";
            return;
        }

        // remove exit code pre
        QString number_wo_xc = number.replace( QRegularExpression("^" + rule_found->exit_code + "|^\\+"), "" );

        // strip country code
        this->country_iso = countryPrefix("","", number_wo_xc);
        if(this->country_iso != "")
            this->country_prefix = countryPrefix(this->country_iso);

        QList<rule>::const_iterator foreign_rule = rule::find(this->country_prefix, "");

        QString number_wo_prefix = number.replace( QRegularExpression("^" + this->country_prefix), "");

        if(foreign_rule != rule::rules.end())
            this->number_local = foreign_rule->trunk_code + number_wo_prefix;
        else
            this->number_local = number_wo_prefix;      // missing rule!

    } else {
        qDebug() << "this is local number.";
        this->number_local = number;

        if(rule_found == rule::rules.end()) {
            qDebug() << "no rule found";
            return ;
        }

        QString number_wo_trunk = number.replace( QRegularExpression("^" + rule_found->trunk_code), "" );
        this->number_international = "+" + location_prefix + number_wo_trunk;
        this->country_prefix = location_prefix;
        this->country_iso = location;
    }
}


phonenumber::operator QMap<QString, QString>() const {
    QMap<QString,QString> params;
    params.insert("telnr", number_local);           // this parameter is always given
    params.insert("telnrInt", number_international );
    params.insert("countryIso", country_iso );
    params.insert("countryPrefix", country_prefix );
    return params;

}

phonenumber::phonenumber(const phonenumber& other):number_local{other.number_local},number_international{other.number_international},country_iso{other.country_iso},country_prefix{other.country_prefix} {
}

const QList<rule> rule::rules = {
    rule(QStringLiteral("Afghanistan"),QStringLiteral("93"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Albania"),QStringLiteral("355"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Algeria"),QStringLiteral("213"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("American Samoa"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Andorra"),QStringLiteral("376"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Angola"),QStringLiteral("244"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Anguilla"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Antigua and Barbuda"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Argentina"),QStringLiteral("54"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Armenia"),QStringLiteral("374"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Aruba"),QStringLiteral("297"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Ascension"),QStringLiteral("247"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Australia"),QStringLiteral("61"),QStringLiteral("0011"),QStringLiteral("0")),
    rule(QStringLiteral("Austria"),QStringLiteral("43"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Azerbaijan"),QStringLiteral("994"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Bahamas"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Bahrain"),QStringLiteral("973"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Bangladesh"),QStringLiteral("880"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Barbados"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Belarus"),QStringLiteral("375"),QStringLiteral("810"),QStringLiteral("80")),
    rule(QStringLiteral("Belgium"),QStringLiteral("32"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Belize"),QStringLiteral("501"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Benin"),QStringLiteral("229"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Bermuda"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Bhutan"),QStringLiteral("975"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Bolivia"),QStringLiteral("591"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Bosnia and Herzegovina"),QStringLiteral("387"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Botswana"),QStringLiteral("267"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Brazil"),QStringLiteral("55"),QStringLiteral("0014"),QStringLiteral("0"),QStringLiteral("16")), //  - Brasil Telecom
    rule(QStringLiteral("Brazil"),QStringLiteral("55"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("British Virgin Islands"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Brunei"),QStringLiteral("673"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Bulgaria"),QStringLiteral("359"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Burkina Faso"),QStringLiteral("226"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Burundi"),QStringLiteral("257"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Cambodia"),QStringLiteral("855"),QStringLiteral("001, 007, 008"),QStringLiteral("0")),
    rule(QStringLiteral("Cameroon"),QStringLiteral("237"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Canada"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Cape Verde"),QStringLiteral("238"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Cayman Islands"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Central African Republic"),QStringLiteral("236"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Chad"),QStringLiteral("235"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Chile"),QStringLiteral("56"),QStringLiteral("1230"), QStringLiteral("0"), QStringLiteral("01")),// - Entel
    rule(QStringLiteral("Chile"),QStringLiteral("56"),QStringLiteral("1230"), QStringLiteral("0"), QStringLiteral("10")),// - Entel
    rule(QStringLiteral("Chile"),QStringLiteral("56"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("China"),QStringLiteral("86"),QStringLiteral("00"),QStringLiteral("0")),

    rule(QStringLiteral("Colombia"),QStringLiteral("57"),QStringLiteral("009"),QStringLiteral("0"), QStringLiteral("123")), //  - Movistar
    rule(QStringLiteral("Colombia"),QStringLiteral("57"),QStringLiteral("00414"),QStringLiteral("0"), QStringLiteral("103")), //  - Tigo
    rule(QStringLiteral("Colombia"),QStringLiteral("57"),QStringLiteral("00414"),QStringLiteral("0"), QStringLiteral("111")), //  - Tigo
    rule(QStringLiteral("Colombia"),QStringLiteral("57"),QStringLiteral("00468"),QStringLiteral("0"), QStringLiteral("130")), //  - Avantel
    //rule(QStringLiteral("Colombia"),QStringLiteral("57"),QStringLiteral("00456"),QStringLiteral("0")), //  - Claro Fixed
    rule(QStringLiteral("Colombia"),QStringLiteral("57"),QStringLiteral("00444"),QStringLiteral("0"),QStringLiteral("101")), //  - Claro Mobile
    rule(QStringLiteral("Colombia"),QStringLiteral("57"),QStringLiteral("00"),QStringLiteral("0")),

    rule(QStringLiteral("Comoros"),QStringLiteral("269"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Congo"),QStringLiteral("242"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Cook Islands"),QStringLiteral("682"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Costa Rica"),QStringLiteral("506"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Croatia"),QStringLiteral("385"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Cuba"),QStringLiteral("53"),QStringLiteral("119"),QStringLiteral("0")),
    rule(QStringLiteral("Curacao"),QStringLiteral("599"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Cyprus"),QStringLiteral("357"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Czech Republic"),QStringLiteral("420"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Democratic Republic of Congo"),QStringLiteral("243"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Denmark"),QStringLiteral("45"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Diego Garcia"),QStringLiteral("246"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Djibouti"),QStringLiteral("253"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Dominica"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Dominican Republic"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("East Timor"),QStringLiteral("670"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Ecuador"),QStringLiteral("593"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Egypt"),QStringLiteral("20"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("El Salvador"),QStringLiteral("503"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Equatorial Guinea"),QStringLiteral("240"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Eritrea"),QStringLiteral("291"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Estonia"),QStringLiteral("372"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Ethiopia"),QStringLiteral("251"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Falkland (Malvinas) Islands"),QStringLiteral("500"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Faroe Islands"),QStringLiteral("298"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Fiji"),QStringLiteral("679"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Finland"),QStringLiteral("358"),QStringLiteral("00, 990, 994, 999"),QStringLiteral("0")),
    rule(QStringLiteral("France"),QStringLiteral("33"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("French Guiana"),QStringLiteral("594"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("French Polynesia"),QStringLiteral("689"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Gabon"),QStringLiteral("241"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Gambia"),QStringLiteral("220"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Georgia"),QStringLiteral("995"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Germany"),QStringLiteral("49"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Ghana"),QStringLiteral("233"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Gibraltar"),QStringLiteral("350"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Greece"),QStringLiteral("30"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Greenland"),QStringLiteral("299"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Grenada"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Guadeloupe"),QStringLiteral("590"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Guam"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Guatemala"),QStringLiteral("502"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Guinea"),QStringLiteral("224"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Guinea-Bissau"),QStringLiteral("245"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Guyana"),QStringLiteral("592"),QStringLiteral("001"),QStringLiteral("")),
    rule(QStringLiteral("Haiti"),QStringLiteral("509"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Honduras"),QStringLiteral("504"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Hong Kong"),QStringLiteral("852"),QStringLiteral("001"),QStringLiteral("")),
    rule(QStringLiteral("Hungary"),QStringLiteral("36"),QStringLiteral("00"),QStringLiteral("06")),
    rule(QStringLiteral("Iceland"),QStringLiteral("354"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("India"),QStringLiteral("91"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Indonesia"),QStringLiteral("62"),QStringLiteral("001"),QStringLiteral("0"),QStringLiteral("01")), // , 008 - Indosat
    rule(QStringLiteral("Indonesia"),QStringLiteral("62"),QStringLiteral("007"),QStringLiteral("0"),QStringLiteral("07")), //  - Telkom
    rule(QStringLiteral("Indonesia"),QStringLiteral("62"),QStringLiteral("007"),QStringLiteral("0"),QStringLiteral("20")), //  - Telkom
    rule(QStringLiteral("Indonesia"),QStringLiteral("62"),QStringLiteral("009"),QStringLiteral("0"),QStringLiteral("99")), //  - Bakrie Telecom
    rule(QStringLiteral("Indonesia"),QStringLiteral("62"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Inmarsat Satellite"),QStringLiteral("870"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Iran"),QStringLiteral("98"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Iraq"),QStringLiteral("964"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Ireland"),QStringLiteral("353"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Iridium Satellite"),QStringLiteral("8816/8817"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Israel"),QStringLiteral("972"),QStringLiteral("00, 012, 013, 014, 018"),QStringLiteral("0")),
    rule(QStringLiteral("Italy"),QStringLiteral("39"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Ivory Coast"),QStringLiteral("225"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Jamaica"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Japan"),QStringLiteral("81"),QStringLiteral("010"),QStringLiteral("0")),
    rule(QStringLiteral("Jordan"),QStringLiteral("962"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Kazakhstan"),QStringLiteral("7"),QStringLiteral("810"),QStringLiteral("8")),
    rule(QStringLiteral("Kenya"),QStringLiteral("254"),QStringLiteral("000 (006 and 007 to Uganda and Tanzania)"),QStringLiteral("0")),
    rule(QStringLiteral("Kiribati"),QStringLiteral("686"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Kuwait"),QStringLiteral("965"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Kyrgyzstan"),QStringLiteral("996"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Laos"),QStringLiteral("856"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Latvia"),QStringLiteral("371"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Lebanon"),QStringLiteral("961"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Lesotho"),QStringLiteral("266"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Liberia"),QStringLiteral("231"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Libya"),QStringLiteral("218"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Liechtenstein"),QStringLiteral("423"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Lithuania"),QStringLiteral("370"),QStringLiteral("00"),QStringLiteral("8")),
    rule(QStringLiteral("Luxembourg"),QStringLiteral("352"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Macau"),QStringLiteral("853"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Macedonia"),QStringLiteral("389"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Madagascar"),QStringLiteral("261"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Malawi"),QStringLiteral("265"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Malaysia"),QStringLiteral("60"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Maldives"),QStringLiteral("960"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Mali"),QStringLiteral("223"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Malta"),QStringLiteral("356"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Marshall Islands"),QStringLiteral("692"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Martinique"),QStringLiteral("596"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Mauritania"),QStringLiteral("222"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Mauritius"),QStringLiteral("230"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Mayotte"),QStringLiteral("262"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Mexico"),QStringLiteral("52"),QStringLiteral("00"),QStringLiteral("01")),
    rule(QStringLiteral("Micronesia"),QStringLiteral("691"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Moldova"),QStringLiteral("373"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Monaco"),QStringLiteral("377"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Mongolia"),QStringLiteral("976"),QStringLiteral("001"),QStringLiteral("0")),
    rule(QStringLiteral("Montenegro"),QStringLiteral("382"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Montserrat"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Morocco"),QStringLiteral("212"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Mozambique"),QStringLiteral("258"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Myanmar"),QStringLiteral("95"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Namibia"),QStringLiteral("264"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Nauru"),QStringLiteral("674"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Nepal"),QStringLiteral("977"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Netherlands"),QStringLiteral("31"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Netherlands Antilles"),QStringLiteral("599"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("New Caledonia"),QStringLiteral("687"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("New Zealand"),QStringLiteral("64"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Nicaragua"),QStringLiteral("505"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Niger"),QStringLiteral("227"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Nigeria"),QStringLiteral("234"),QStringLiteral("009"),QStringLiteral("0")),
    rule(QStringLiteral("Niue"),QStringLiteral("683"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Norfolk Island"),QStringLiteral("6723"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("North Korea"),QStringLiteral("850"),QStringLiteral("99"),QStringLiteral("")),
    rule(QStringLiteral("Northern Marianas"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Norway"),QStringLiteral("47"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Oman"),QStringLiteral("968"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Pakistan"),QStringLiteral("92"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Palau"),QStringLiteral("680"),QStringLiteral("011"),QStringLiteral("")),
    rule(QStringLiteral("Palestine"),QStringLiteral("970"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Panama"),QStringLiteral("507"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Papua New Guinea"),QStringLiteral("675"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Paraguay"),QStringLiteral("595"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Peru"),QStringLiteral("51"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Philippines"),QStringLiteral("63"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Poland"),QStringLiteral("48"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Portugal"),QStringLiteral("351"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Puerto Rico"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Qatar"),QStringLiteral("974"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Reunion"),QStringLiteral("262"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Romania"),QStringLiteral("40"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Russian Federation"),QStringLiteral("7"),QStringLiteral("810"),QStringLiteral("8")),
    rule(QStringLiteral("Rwanda"),QStringLiteral("250"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Saint Helena"),QStringLiteral("290"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Saint Kitts and Nevis"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Saint Lucia"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Saint Barthelemy"),QStringLiteral("590"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Saint Martin (French part)"),QStringLiteral("590"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Saint Pierre and Miquelon"),QStringLiteral("508"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Saint Vincent and the Grenadines"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Samoa"),QStringLiteral("685"),QStringLiteral("0"),QStringLiteral("")),
    rule(QStringLiteral("San Marino"),QStringLiteral("378"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Sao Tome and Principe"),QStringLiteral("239"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Saudi Arabia"),QStringLiteral("966"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Senegal"),QStringLiteral("221"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Serbia"),QStringLiteral("381"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Seychelles"),QStringLiteral("248"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Sierra Leone"),QStringLiteral("232"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Singapore"),QStringLiteral("65"),QStringLiteral("001, 008"),QStringLiteral("")),
    rule(QStringLiteral("Sint Maarten"),QStringLiteral("1"),QStringLiteral("00"),QStringLiteral("1")),
    rule(QStringLiteral("Slovakia"),QStringLiteral("421"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Slovenia"),QStringLiteral("386"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Solomon Islands"),QStringLiteral("677"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Somalia"),QStringLiteral("252"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("South Africa"),QStringLiteral("27"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("South Korea"),QStringLiteral("82"),QStringLiteral("001, 002"),QStringLiteral("0")),
    rule(QStringLiteral("South Sudan"),QStringLiteral("211"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Spain"),QStringLiteral("34"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Sri Lanka"),QStringLiteral("94"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Sudan"),QStringLiteral("249"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Suriname"),QStringLiteral("597"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Swaziland"),QStringLiteral("268"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Sweden"),QStringLiteral("46"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Switzerland"),QStringLiteral("41"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Syria"),QStringLiteral("963"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Taiwan"),QStringLiteral("886"),QStringLiteral("002"),QStringLiteral("0")),
    rule(QStringLiteral("Tajikistan"),QStringLiteral("992"),QStringLiteral("810"),QStringLiteral("8")),
    rule(QStringLiteral("Tanzania"),QStringLiteral("255"),QStringLiteral("000"),QStringLiteral("0")),
    rule(QStringLiteral("Thailand"),QStringLiteral("66"),QStringLiteral("001"),QStringLiteral("0")),
    rule(QStringLiteral("Thuraya Satellite"),QStringLiteral("882 16"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Togo"),QStringLiteral("228"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Tokelau"),QStringLiteral("690"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Tonga"),QStringLiteral("676"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Trinidad and Tobago"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Tunisia"),QStringLiteral("216"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Turkey"),QStringLiteral("90"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Turkmenistan"),QStringLiteral("993"),QStringLiteral("810"),QStringLiteral("8")),
    rule(QStringLiteral("Turks and Caicos Islands"),QStringLiteral("1"),QStringLiteral("0"),QStringLiteral("1")),
    rule(QStringLiteral("Tuvalu"),QStringLiteral("688"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Uganda"),QStringLiteral("256"),QStringLiteral("000"),QStringLiteral("0")),
    rule(QStringLiteral("Ukraine"),QStringLiteral("380"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("United Arab Emirates"),QStringLiteral("971"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("United Kingdom"),QStringLiteral("44"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("United States of America"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("U.S. Virgin Islands"),QStringLiteral("1"),QStringLiteral("011"),QStringLiteral("1")),
    rule(QStringLiteral("Uruguay"),QStringLiteral("598"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Uzbekistan"),QStringLiteral("998"),QStringLiteral("810"),QStringLiteral("8")),
    rule(QStringLiteral("Vanuatu"),QStringLiteral("678"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Vatican City"),QStringLiteral("379, 39"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Venezuela"),QStringLiteral("58"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Vietnam"),QStringLiteral("84"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Wallis and Futuna"),QStringLiteral("681"),QStringLiteral("00"),QStringLiteral("")),
    rule(QStringLiteral("Yemen"),QStringLiteral("967"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Zambia"),QStringLiteral("260"),QStringLiteral("00"),QStringLiteral("0")),
    rule(QStringLiteral("Zimbabwe"),QStringLiteral("263"),QStringLiteral("00"),QStringLiteral("0"))
};

rule::rule(const QString &country_name, const QString &country_prefix, const QString &exit_code, const QString &trunk_code, const QString &network_code)
    :country_name{country_name},country_prefix{country_prefix},exit_code{exit_code},trunk_code{trunk_code},network_code{network_code}
{
}


QList<rule>::const_iterator rule::find(const QString &country_prefix, const QString &mnc) {

    auto iter = std::find_if(rules.begin(),rules.end(),[&](const rule &r){
        return (r.country_prefix == country_prefix &&
                (r.network_code == mnc || r.network_code == ""));
    });

    if(iter != rules.end()){
        qDebug() << "match!" << iter->country_name;
    }
    return iter;
}



QString phonenumber::mobilecc_to_iso32662(int code) {

    static const QHash<int, QString> mccList{
        {202,QStringLiteral("GR")},
        {204,QStringLiteral("NL")},
        {206,QStringLiteral("BE")},
        {208,QStringLiteral("FR")},
        {212,QStringLiteral("MC")},
        {213,QStringLiteral("AD")},
        {214,QStringLiteral("ES")},
        {216,QStringLiteral("HU")},
        {218,QStringLiteral("BA")},
        {219,QStringLiteral("HR")},
        {220,QStringLiteral("RS")},
        {222,QStringLiteral("IT")},
        {226,QStringLiteral("RO")},
        {228,QStringLiteral("CH")},
        {230,QStringLiteral("CZ")},
        {231,QStringLiteral("SK")},
        {232,QStringLiteral("AT")},
        {234,QStringLiteral("GB")},
        {235,QStringLiteral("GB")},
        {238,QStringLiteral("DK")},
        {240,QStringLiteral("SE")},
        {242,QStringLiteral("NO")},
        {244,QStringLiteral("FI")},
        {246,QStringLiteral("LT")},
        {247,QStringLiteral("LV")},
        {248,QStringLiteral("EE")},
        {250,QStringLiteral("RU")},
        {255,QStringLiteral("UA")},
        {257,QStringLiteral("BY")},
        {259,QStringLiteral("MD")},
        {260,QStringLiteral("PL")},
        {262,QStringLiteral("DE")},
        {266,QStringLiteral("GI")},
        {268,QStringLiteral("PT")},
        {270,QStringLiteral("LU")},
        {272,QStringLiteral("IE")},
        {274,QStringLiteral("IS")},
        {276,QStringLiteral("AL")},
        {278,QStringLiteral("MT")},
        {280,QStringLiteral("CY")},
        {282,QStringLiteral("GE")},
        {283,QStringLiteral("AM")},
        {284,QStringLiteral("BG")},
        {286,QStringLiteral("TR")},
        {288,QStringLiteral("FO")},
        {289,QStringLiteral("GE")},
        {290,QStringLiteral("GL")},
        {292,QStringLiteral("SM")},
        {293,QStringLiteral("SI")},
        {294,QStringLiteral("MK")},
        {295,QStringLiteral("LI")},
        {297,QStringLiteral("ME")},
        {302,QStringLiteral("CA")},
        {308,QStringLiteral("PM")},
        {310,QStringLiteral("GU")},
        {310,QStringLiteral("US")},
        {311,QStringLiteral("GU")},
        {311,QStringLiteral("US")},
        {312,QStringLiteral("US")},
        {316,QStringLiteral("US")},
        {330,QStringLiteral("PR")},
        {334,QStringLiteral("MX")},
        {338,QStringLiteral("JM")},
        {340,QStringLiteral("FG")},
        {340,QStringLiteral("GP")},
        {340,QStringLiteral("MQ")},
        {342,QStringLiteral("BB")},
        {344,QStringLiteral("AG")},
        {346,QStringLiteral("KY")},
        {348,QStringLiteral("VG")},
        {350,QStringLiteral("BM")},
        {352,QStringLiteral("GD")},
        {354,QStringLiteral("MS")},
        {356,QStringLiteral("KN")},
        {358,QStringLiteral("LC")},
        {360,QStringLiteral("VC")},
        {362,QStringLiteral("AN")},
        {362,QStringLiteral("CW")},
        {363,QStringLiteral("AW")},
        {364,QStringLiteral("BS")},
        {365,QStringLiteral("AI")},
        {366,QStringLiteral("DM")},
        {368,QStringLiteral("CU")},
        {370,QStringLiteral("DO")},
        {372,QStringLiteral("HT")},
        {374,QStringLiteral("TT")},
        {376,QStringLiteral("TC")},
        {376,QStringLiteral("VI")},
        {400,QStringLiteral("AZ")},
        {401,QStringLiteral("KZ")},
        {402,QStringLiteral("BT")},
        {404,QStringLiteral("IN")},
        {405,QStringLiteral("IN")},
        {410,QStringLiteral("PK")},
        {412,QStringLiteral("AF")},
        {413,QStringLiteral("LK")},
        {414,QStringLiteral("MM")},
        {415,QStringLiteral("LB")},
        {416,QStringLiteral("JO")},
        {417,QStringLiteral("SY")},
        {418,QStringLiteral("IQ")},
        {419,QStringLiteral("KW")},
        {420,QStringLiteral("SA")},
        {421,QStringLiteral("YE")},
        {422,QStringLiteral("OM")},
        {424,QStringLiteral("AE")},
        {425,QStringLiteral("IL")},
        {425,QStringLiteral("PS")},
        {426,QStringLiteral("BH")},
        {427,QStringLiteral("QA")},
        {428,QStringLiteral("MN")},
        {429,QStringLiteral("NP")},
        {430,QStringLiteral("AE")},
        {431,QStringLiteral("AE")},
        {432,QStringLiteral("IR")},
        {434,QStringLiteral("UZ")},
        {436,QStringLiteral("TK")},
        {437,QStringLiteral("KG")},
        {438,QStringLiteral("TM")},
        {440,QStringLiteral("JP")},
        {441,QStringLiteral("JP")},
        {450,QStringLiteral("KR")},
        {452,QStringLiteral("VN")},
        {454,QStringLiteral("HK")},
        {455,QStringLiteral("MO")},
        {456,QStringLiteral("KH")},
        {457,QStringLiteral("LA")},
        {460,QStringLiteral("CN")},
        {466,QStringLiteral("TW")},
        {467,QStringLiteral("KP")},
        {470,QStringLiteral("BD")},
        {472,QStringLiteral("MV")},
        {502,QStringLiteral("MY")},
        {505,QStringLiteral("AU")},
        {510,QStringLiteral("ID")},
        {514,QStringLiteral("TP")},
        {515,QStringLiteral("PH")},
        {520,QStringLiteral("TH")},
        {525,QStringLiteral("SG")},
        {528,QStringLiteral("BN")},
        {530,QStringLiteral("NZ")},
        {537,QStringLiteral("PG")},
        {539,QStringLiteral("TO")},
        {540,QStringLiteral("SB")},
        {541,QStringLiteral("VU")},
        {542,QStringLiteral("FJ")},
        {544,QStringLiteral("AS")},
        {545,QStringLiteral("KI")},
        {546,QStringLiteral("NC")},
        {547,QStringLiteral("PF")},
        {548,QStringLiteral("CK")},
        {549,QStringLiteral("WS")},
        {550,QStringLiteral("FM")},
        {552,QStringLiteral("PW")},
        {553,QStringLiteral("TV")},
        {555,QStringLiteral("NU")},
        {602,QStringLiteral("EG")},
        {603,QStringLiteral("DZ")},
        {604,QStringLiteral("MA")},
        {605,QStringLiteral("TN")},
        {606,QStringLiteral("LY")},
        {607,QStringLiteral("GM")},
        {608,QStringLiteral("SN")},
        {609,QStringLiteral("MR")},
        {610,QStringLiteral("ML")},
        {611,QStringLiteral("GN")},
        {612,QStringLiteral("CI")},
        {613,QStringLiteral("BF")},
        {614,QStringLiteral("NE")},
        {615,QStringLiteral("TG")},
        {616,QStringLiteral("BJ")},
        {617,QStringLiteral("MU")},
        {618,QStringLiteral("LR")},
        {619,QStringLiteral("SL")},
        {620,QStringLiteral("GH")},
        {621,QStringLiteral("NG")},
        {622,QStringLiteral("TD")},
        {623,QStringLiteral("CF")},
        {624,QStringLiteral("CM")},
        {625,QStringLiteral("CV")},
        {626,QStringLiteral("ST")},
        {627,QStringLiteral("GQ")},
        {628,QStringLiteral("GA")},
        {629,QStringLiteral("CG")},
        {630,QStringLiteral("CD")},
        {631,QStringLiteral("AO")},
        {632,QStringLiteral("GW")},
        {633,QStringLiteral("SC")},
        {634,QStringLiteral("SD")},
        {635,QStringLiteral("RW")},
        {636,QStringLiteral("ET")},
        {637,QStringLiteral("SO")},
        {638,QStringLiteral("DJ")},
        {639,QStringLiteral("KE")},
        {640,QStringLiteral("TZ")},
        {641,QStringLiteral("UG")},
        {642,QStringLiteral("BI")},
        {643,QStringLiteral("MZ")},
        {645,QStringLiteral("ZM")},
        {646,QStringLiteral("MG")},
        {647,QStringLiteral("RE")},
        {648,QStringLiteral("ZW")},
        {649,QStringLiteral("NA")},
        {650,QStringLiteral("MW")},
        {651,QStringLiteral("LS")},
        {652,QStringLiteral("BW")},
        {653,QStringLiteral("SZ")},
        {654,QStringLiteral("KM")},
        {655,QStringLiteral("ZA")},
        {657,QStringLiteral("ER")},
        {659,QStringLiteral("SS")},
        {702,QStringLiteral("BZ")},
        {704,QStringLiteral("GT")},
        {706,QStringLiteral("SV")},
        {708,QStringLiteral("HN")},
        {710,QStringLiteral("NI")},
        {712,QStringLiteral("CR")},
        {714,QStringLiteral("PA")},
        {716,QStringLiteral("PE")},
        {722,QStringLiteral("AR")},
        {724,QStringLiteral("BR")},
        {730,QStringLiteral("CL")},
        {732,QStringLiteral("CO")},
        {734,QStringLiteral("VE")},
        {736,QStringLiteral("BO")},
        {738,QStringLiteral("GY")},
        {740,QStringLiteral("EC")},
        {744,QStringLiteral("PY")},
        {746,QStringLiteral("SR")},
        {748,QStringLiteral("UY")},
        {750,QStringLiteral("FK")}
    };


    if(mccList.contains(code))
        return mccList[code];

    return "";

}

QString phonenumber::countryPrefix(QString country, QString prefix, QString number) {
    typedef QPair<QString,QString> pz;
    static const QList<pz> c{
        pz("AF", "93"),pz(QStringLiteral("AF"), QStringLiteral("93")),
                pz(QStringLiteral("AL"), QStringLiteral("355")),
                pz(QStringLiteral("DZ"), QStringLiteral("213")),
                pz(QStringLiteral("AS"), QStringLiteral("1684")),
                pz(QStringLiteral("AD"), QStringLiteral("376")),
                pz(QStringLiteral("AO"), QStringLiteral("244")),
                pz(QStringLiteral("AI"), QStringLiteral("1264")),

                pz(QStringLiteral("AG"), QStringLiteral("1268")),
                pz(QStringLiteral("AR"), QStringLiteral("54")),
                pz(QStringLiteral("AM"), QStringLiteral("374")),
                pz(QStringLiteral("AW"), QStringLiteral("297")),

                pz(QStringLiteral("AU"), QStringLiteral("61")),
                pz(QStringLiteral("CX"), QStringLiteral("61")),
                pz(QStringLiteral("CC"), QStringLiteral("61")),

                pz(QStringLiteral("AT"), QStringLiteral("43")),
                pz(QStringLiteral("AZ"), QStringLiteral("994")),
                pz(QStringLiteral("BS"), QStringLiteral("1242")),
                pz(QStringLiteral("BH"), QStringLiteral("973")),
                pz(QStringLiteral("BD"), QStringLiteral("880")),
                pz(QStringLiteral("BB"), QStringLiteral("1246")),
                pz(QStringLiteral("BY"), QStringLiteral("375")),
                pz(QStringLiteral("BE"), QStringLiteral("32")),
                pz(QStringLiteral("BZ"), QStringLiteral("501")),
                pz(QStringLiteral("BJ"), QStringLiteral("229")),
                pz(QStringLiteral("BM"), QStringLiteral("1441")),
                pz(QStringLiteral("BT"), QStringLiteral("975")),
                pz(QStringLiteral("BO"), QStringLiteral("591")),
                pz(QStringLiteral("BA"), QStringLiteral("387")),
                pz(QStringLiteral("BW"), QStringLiteral("267")),
                pz(QStringLiteral("BR"), QStringLiteral("55")),
                pz(QStringLiteral("IO"), QStringLiteral("246")),
                pz(QStringLiteral("BN"), QStringLiteral("673")),
                pz(QStringLiteral("BG"), QStringLiteral("359")),
                pz(QStringLiteral("BF"), QStringLiteral("226")),
                pz(QStringLiteral("BI"), QStringLiteral("257")),
                pz(QStringLiteral("KH"), QStringLiteral("855")),
                pz(QStringLiteral("CM"), QStringLiteral("237")),
                pz(QStringLiteral("CV"), QStringLiteral("238")),
                pz(QStringLiteral("KY"), QStringLiteral("1345")),
                pz(QStringLiteral("CF"), QStringLiteral("236")),
                pz(QStringLiteral("TD"), QStringLiteral("235")),
                pz(QStringLiteral("CL"), QStringLiteral("56")),
                pz(QStringLiteral("CN"), QStringLiteral("86")),
                pz(QStringLiteral("CO"), QStringLiteral("57")),
                pz(QStringLiteral("KM"), QStringLiteral("269")),
                pz(QStringLiteral("CG"), QStringLiteral("242")),
                pz(QStringLiteral("CD"), QStringLiteral("243")),
                pz(QStringLiteral("CK"), QStringLiteral("682")),
                pz(QStringLiteral("CR"), QStringLiteral("506")),
                pz(QStringLiteral("HR"), QStringLiteral("385")),
                pz(QStringLiteral("CU"), QStringLiteral("53")),

                pz(QStringLiteral("CW"), QStringLiteral("599")),
                pz(QStringLiteral("BQ"), QStringLiteral("599")),

                pz(QStringLiteral("CY"), QStringLiteral("357")),
                pz(QStringLiteral("CZ"), QStringLiteral("420")),
                pz(QStringLiteral("CI"), QStringLiteral("225")),
                pz(QStringLiteral("DK"), QStringLiteral("45")),
                pz(QStringLiteral("DJ"), QStringLiteral("253")),
                pz(QStringLiteral("DM"), QStringLiteral("1767")),
                pz(QStringLiteral("DO"), QStringLiteral("1809")),
                pz(QStringLiteral("DO"), QStringLiteral("1829")),
                pz(QStringLiteral("DO"), QStringLiteral("1849")),
                pz(QStringLiteral("EC"), QStringLiteral("593")),
                pz(QStringLiteral("EG"), QStringLiteral("20")),
                pz(QStringLiteral("SV"), QStringLiteral("503")),
                pz(QStringLiteral("GQ"), QStringLiteral("240")),
                pz(QStringLiteral("ER"), QStringLiteral("291")),
                pz(QStringLiteral("EE"), QStringLiteral("372")),
                pz(QStringLiteral("ET"), QStringLiteral("251")),

                pz(QStringLiteral("FK"), QStringLiteral("500")),
                pz(QStringLiteral("GS"), QStringLiteral("500")),

                pz(QStringLiteral("FO"), QStringLiteral("298")),
                pz(QStringLiteral("FJ"), QStringLiteral("679")),

                pz(QStringLiteral("FI"), QStringLiteral("358")),
                pz(QStringLiteral("AX"), QStringLiteral("358")),

                pz(QStringLiteral("FR"), QStringLiteral("33")),
                pz(QStringLiteral("GF"), QStringLiteral("594")),
                pz(QStringLiteral("PF"), QStringLiteral("689")),

                pz(QStringLiteral("GA"), QStringLiteral("241")),
                pz(QStringLiteral("GM"), QStringLiteral("220")),
                pz(QStringLiteral("GE"), QStringLiteral("995")),
                pz(QStringLiteral("DE"), QStringLiteral("49")),
                pz(QStringLiteral("GH"), QStringLiteral("233")),
                pz(QStringLiteral("GI"), QStringLiteral("350")),
                pz(QStringLiteral("GR"), QStringLiteral("30")),
                pz(QStringLiteral("GL"), QStringLiteral("299")),
                pz(QStringLiteral("GD"), QStringLiteral("1473")),

                pz(QStringLiteral("GP"), QStringLiteral("590")),
                pz(QStringLiteral("BL"), QStringLiteral("590")),
                pz(QStringLiteral("MF"), QStringLiteral("590")),

                pz(QStringLiteral("GU"), QStringLiteral("1671")),
                pz(QStringLiteral("GT"), QStringLiteral("502")),
                pz(QStringLiteral("GN"), QStringLiteral("224")),
                pz(QStringLiteral("GW"), QStringLiteral("245")),
                pz(QStringLiteral("GY"), QStringLiteral("592")),
                pz(QStringLiteral("HT"), QStringLiteral("509")),
                pz(QStringLiteral("VA"), QStringLiteral("3906")),
                pz(QStringLiteral("HN"), QStringLiteral("504")),
                pz(QStringLiteral("HK"), QStringLiteral("852")),
                pz(QStringLiteral("HU"), QStringLiteral("36")),
                pz(QStringLiteral("IS"), QStringLiteral("354")),
                pz(QStringLiteral("IN"), QStringLiteral("91")),
                pz(QStringLiteral("ID"), QStringLiteral("62")),
                pz(QStringLiteral("IR"), QStringLiteral("98")),
                pz(QStringLiteral("IQ"), QStringLiteral("964")),
                pz(QStringLiteral("IE"), QStringLiteral("353")),
                pz(QStringLiteral("IL"), QStringLiteral("972")),
                pz(QStringLiteral("IT"), QStringLiteral("39")),
                pz(QStringLiteral("JM"), QStringLiteral("1876")),
                pz(QStringLiteral("JP"), QStringLiteral("81")),
                pz(QStringLiteral("JO"), QStringLiteral("962")),
                pz(QStringLiteral("KE"), QStringLiteral("254")),
                pz(QStringLiteral("KI"), QStringLiteral("686")),
                pz(QStringLiteral("KP"), QStringLiteral("850")),
                pz(QStringLiteral("KR"), QStringLiteral("82")),
                pz(QStringLiteral("KW"), QStringLiteral("965")),
                pz(QStringLiteral("KG"), QStringLiteral("996")),
                pz(QStringLiteral("LA"), QStringLiteral("856")),
                pz(QStringLiteral("LV"), QStringLiteral("371")),
                pz(QStringLiteral("LB"), QStringLiteral("961")),
                pz(QStringLiteral("LS"), QStringLiteral("266")),
                pz(QStringLiteral("LR"), QStringLiteral("231")),
                pz(QStringLiteral("LY"), QStringLiteral("218")),
                pz(QStringLiteral("LI"), QStringLiteral("423")),
                pz(QStringLiteral("LT"), QStringLiteral("370")),
                pz(QStringLiteral("LU"), QStringLiteral("352")),
                pz(QStringLiteral("MO"), QStringLiteral("853")),
                pz(QStringLiteral("MK"), QStringLiteral("389")),
                pz(QStringLiteral("MG"), QStringLiteral("261")),
                pz(QStringLiteral("MW"), QStringLiteral("265")),
                pz(QStringLiteral("MY"), QStringLiteral("60")),
                pz(QStringLiteral("MV"), QStringLiteral("960")),
                pz(QStringLiteral("ML"), QStringLiteral("223")),
                pz(QStringLiteral("MT"), QStringLiteral("356")),
                pz(QStringLiteral("MH"), QStringLiteral("692")),
                pz(QStringLiteral("MQ"), QStringLiteral("596")),
                pz(QStringLiteral("MR"), QStringLiteral("222")),
                pz(QStringLiteral("MU"), QStringLiteral("230")),

                pz(QStringLiteral("YT"), QStringLiteral("262")),
                pz(QStringLiteral("TF"), QStringLiteral("262")),
                pz(QStringLiteral("RE"), QStringLiteral("262")),

                pz(QStringLiteral("MX"), QStringLiteral("52")),
                pz(QStringLiteral("FM"), QStringLiteral("691")),
                pz(QStringLiteral("MD"), QStringLiteral("373")),
                pz(QStringLiteral("MC"), QStringLiteral("377")),
                pz(QStringLiteral("MN"), QStringLiteral("976")),
                pz(QStringLiteral("ME"), QStringLiteral("382")),
                pz(QStringLiteral("MS"), QStringLiteral("1664")),

                pz(QStringLiteral("MA"), QStringLiteral("212")),
                pz(QStringLiteral("EH"), QStringLiteral("212")),

                pz(QStringLiteral("MZ"), QStringLiteral("258")),
                pz(QStringLiteral("MM"), QStringLiteral("95")),
                pz(QStringLiteral("NA"), QStringLiteral("264")),
                pz(QStringLiteral("NR"), QStringLiteral("674")),
                pz(QStringLiteral("NP"), QStringLiteral("977")),
                pz(QStringLiteral("NL"), QStringLiteral("31")),
                pz(QStringLiteral("NC"), QStringLiteral("687")),
                pz(QStringLiteral("NZ"), QStringLiteral("64")),
                pz(QStringLiteral("NI"), QStringLiteral("505")),
                pz(QStringLiteral("NE"), QStringLiteral("227")),
                pz(QStringLiteral("NG"), QStringLiteral("234")),
                pz(QStringLiteral("NU"), QStringLiteral("683")),

                pz(QStringLiteral("NF"), QStringLiteral("672")),
                pz(QStringLiteral("AQ"), QStringLiteral("672")),
                pz(QStringLiteral("HM"), QStringLiteral("672")),

                pz(QStringLiteral("MP"), QStringLiteral("1670")),

                pz(QStringLiteral("NO"), QStringLiteral("47")),
                pz(QStringLiteral("SJ"), QStringLiteral("47")),
                pz(QStringLiteral("BV"), QStringLiteral("47")),

                pz(QStringLiteral("OM"), QStringLiteral("968")),
                pz(QStringLiteral("PK"), QStringLiteral("92")),
                pz(QStringLiteral("PW"), QStringLiteral("680")),
                pz(QStringLiteral("PS"), QStringLiteral("970")),
                pz(QStringLiteral("PA"), QStringLiteral("507")),
                pz(QStringLiteral("PG"), QStringLiteral("675")),
                pz(QStringLiteral("PY"), QStringLiteral("595")),
                pz(QStringLiteral("PE"), QStringLiteral("51")),
                pz(QStringLiteral("PH"), QStringLiteral("63")),
                pz(QStringLiteral("PN"), QStringLiteral("870")),
                pz(QStringLiteral("PL"), QStringLiteral("48")),
                pz(QStringLiteral("PT"), QStringLiteral("351")),


                pz(QStringLiteral("QA"), QStringLiteral("974")),
                pz(QStringLiteral("RO"), QStringLiteral("40")),

                pz(QStringLiteral("RU"), QStringLiteral("7")),
                pz(QStringLiteral("KZ"), QStringLiteral("7")),
                pz(QStringLiteral("RW"), QStringLiteral("250")),


                pz(QStringLiteral("SH"), QStringLiteral("290")),
                pz(QStringLiteral("KN"), QStringLiteral("1869")),
                pz(QStringLiteral("LC"), QStringLiteral("1758")),

                pz(QStringLiteral("PM"), QStringLiteral("508")),
                pz(QStringLiteral("VC"), QStringLiteral("1784")),
                pz(QStringLiteral("WS"), QStringLiteral("685")),
                pz(QStringLiteral("SM"), QStringLiteral("378")),
                pz(QStringLiteral("ST"), QStringLiteral("239")),
                pz(QStringLiteral("SA"), QStringLiteral("966")),
                pz(QStringLiteral("SN"), QStringLiteral("221")),
                pz(QStringLiteral("RS"), QStringLiteral("381")),
                pz(QStringLiteral("SC"), QStringLiteral("248")),
                pz(QStringLiteral("SL"), QStringLiteral("232")),
                pz(QStringLiteral("SG"), QStringLiteral("65")),
                pz(QStringLiteral("SX"), QStringLiteral("1721")),
                pz(QStringLiteral("SK"), QStringLiteral("421")),
                pz(QStringLiteral("SI"), QStringLiteral("386")),
                pz(QStringLiteral("SB"), QStringLiteral("677")),
                pz(QStringLiteral("SO"), QStringLiteral("252")),
                pz(QStringLiteral("ZA"), QStringLiteral("27")),

                pz(QStringLiteral("SS"), QStringLiteral("211")),
                pz(QStringLiteral("ES"), QStringLiteral("34")),
                pz(QStringLiteral("LK"), QStringLiteral("94")),
                pz(QStringLiteral("SD"), QStringLiteral("249")),
                pz(QStringLiteral("SR"), QStringLiteral("597")),

                pz(QStringLiteral("SZ"), QStringLiteral("268")),
                pz(QStringLiteral("SE"), QStringLiteral("46")),
                pz(QStringLiteral("CH"), QStringLiteral("41")),
                pz(QStringLiteral("SY"), QStringLiteral("963")),
                pz(QStringLiteral("TW"), QStringLiteral("886")),
                pz(QStringLiteral("TJ"), QStringLiteral("992")),
                pz(QStringLiteral("TZ"), QStringLiteral("255")),
                pz(QStringLiteral("TH"), QStringLiteral("66")),
                pz(QStringLiteral("TL"), QStringLiteral("670")),
                pz(QStringLiteral("TG"), QStringLiteral("228")),
                pz(QStringLiteral("TK"), QStringLiteral("690")),
                pz(QStringLiteral("TO"), QStringLiteral("676")),
                pz(QStringLiteral("TT"), QStringLiteral("1868")),
                pz(QStringLiteral("TN"), QStringLiteral("216")),
                pz(QStringLiteral("TR"), QStringLiteral("90")),
                pz(QStringLiteral("TM"), QStringLiteral("993")),
                pz(QStringLiteral("TC"), QStringLiteral("1649")),
                pz(QStringLiteral("TV"), QStringLiteral("688")),
                pz(QStringLiteral("UG"), QStringLiteral("256")),
                pz(QStringLiteral("UA"), QStringLiteral("380")),
                pz(QStringLiteral("AE"), QStringLiteral("971")),

                pz(QStringLiteral("GB"), QStringLiteral("44")),
                pz(QStringLiteral("GG"), QStringLiteral("44")),
                pz(QStringLiteral("IM"), QStringLiteral("44")),
                pz(QStringLiteral("JE"), QStringLiteral("44")),

                pz(QStringLiteral("US"), QStringLiteral("1")),
                pz(QStringLiteral("CA"), QStringLiteral("1")),
                pz(QStringLiteral("PR"), QStringLiteral("1")),

                pz(QStringLiteral("UM"), QStringLiteral("2")),
                pz(QStringLiteral("UY"), QStringLiteral("598")),
                pz(QStringLiteral("UZ"), QStringLiteral("998")),
                pz(QStringLiteral("VU"), QStringLiteral("678")),
                pz(QStringLiteral("VE"), QStringLiteral("58")),
                pz(QStringLiteral("VN"), QStringLiteral("84")),
                pz(QStringLiteral("VG"), QStringLiteral("1284")),
                pz(QStringLiteral("VI"), QStringLiteral("1340")),
                pz(QStringLiteral("WF"), QStringLiteral("681")),
                pz(QStringLiteral("YE"), QStringLiteral("967")),
                pz(QStringLiteral("ZM"), QStringLiteral("260")),
                pz(QStringLiteral("ZW"), QStringLiteral("263")),
    };

    if(!country.isEmpty()) {

        auto iter = std::find_if(c.begin(),c.end(),[&](const pz &elem){
            return elem.first == country;
        });

        if(iter != c.end()){
            return iter->second;
        }

        return "";
    }

    if(!prefix.isEmpty()) {

        auto iter = std::find_if(c.begin(),c.end(),[&](const pz &elem){
            return elem.second == prefix;
        });

        if(iter != c.end()){
            return iter->first;
        }

        return "";
    }

    if(!number.isEmpty()) {
        pz bestCandidate;

        for(const auto &it : c) {

            if(number.startsWith( it.second ) && it.second.length() > bestCandidate.second.length() ) {
                bestCandidate = it;
            }
        }


        return bestCandidate.first;
    }

    return "";
}


QString phonenumber::process(QString number) {
    QRegularExpression filter("[^0-9]");

    qDebug() << "normalized number = " << number.replace(filter, "").right(8);

    return number.replace(filter, "").right(8);
}

QString phonenumber::convertNumber(QString number, QString numberCountryIso, QString locationIso) {

    QString locationCountry = locationIso.toLower();

    if(numberCountryIso.toLower() != locationCountry) {
        phonenumber pn(number, numberCountryIso, "");
        return pn.number_international;
    } else {
        // number is local
        return number;
    }
}
