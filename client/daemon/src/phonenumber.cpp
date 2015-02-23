#include "phonenumber.h"
#include <QDebug>

#include <QRegularExpression>

phonenumber::phonenumber(QString number, QString location, QString mnc) {

    number = number.replace(QRegularExpression("[^+0-9]"), "");

    QString location_prefix = countryPrefix(location);

    qDebug() << "location prefix" << location_prefix;

    rule *r =
    rule::find(location_prefix, mnc);

    if(!r) {
        qDebug() << "no rule found";
        return ;
    }


    if(number.startsWith(r->exit_code) || number.startsWith("+")) {
        qDebug() << "this is international number!";

        this->number_international = number;

        // remove exit code pre
        QString number_wo_xc = number.replace( QRegularExpression("^" + r->exit_code + "|^\\+"), "" );


        // strip country code
        this->country_iso = countryPrefix("","", number_wo_xc);
        if(this->country_iso != "")
            this->country_prefix = countryPrefix(this->country_iso);

        rule *foreign_rule = rule::find(this->country_prefix, "");

        QString number_wo_prefix = number.replace( QRegularExpression("^" + this->country_prefix), "");

        if(foreign_rule)
            this->number_local = foreign_rule->trunk_code + number_wo_prefix;
        else
            this->number_local = number_wo_prefix;      // missing rule!

    } else {
        qDebug() << "this is local number.";
        this->number_local = number;

        QString number_wo_trunk = number.replace( QRegularExpression("^" + r->trunk_code), "" );
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

phonenumber::phonenumber(const phonenumber& other) {
    this->number_local = other.number_local;
    this->number_international = other.number_international;
    this->country_iso = other.country_iso;
    this->country_prefix = other.country_prefix;
}


void rule::initialize() {
    foreach(rule *r, rules) delete r;
    rules.clear();

    new rule("Afghanistan","93","00","0");
    new rule("Albania","355","00","0");
    new rule("Algeria","213","00","0");
    new rule("American Samoa","1","011","1");
    new rule("Andorra","376","00","");
    new rule("Angola","244","00","");
    new rule("Anguilla","1","011","1");
    new rule("Antigua and Barbuda","1","011","1");
    new rule("Argentina","54","00","0");
    new rule("Armenia","374","00","0");
    new rule("Aruba","297","00","");
    new rule("Ascension","247","00","");
    new rule("Australia","61","0011","0");
    new rule("Austria","43","00","0");
    new rule("Azerbaijan","994","00","0");
    new rule("Bahamas","1","011","1");
    new rule("Bahrain","973","00","");
    new rule("Bangladesh","880","00","0");
    new rule("Barbados","1","011","1");
    new rule("Belarus","375","810","80");
    new rule("Belgium","32","00","0");
    new rule("Belize","501","00","");
    new rule("Benin","229","00","");
    new rule("Bermuda","1","011","1");
    new rule("Bhutan","975","00","");
    new rule("Bolivia","591","00","0");
    new rule("Bosnia and Herzegovina","387","00","0");
    new rule("Botswana","267","00","");
    new rule("Brazil","55","0014","0","16"); //  - Brasil Telecom
    new rule("Brazil","55","00","0");
    new rule("British Virgin Islands","1","011","1");
    new rule("Brunei","673","00","");
    new rule("Bulgaria","359","00","0");
    new rule("Burkina Faso","226","00","");
    new rule("Burundi","257","00","");
    new rule("Cambodia","855","001, 007, 008","0");
    new rule("Cameroon","237","00","");
    new rule("Canada","1","011","1");
    new rule("Cape Verde","238","00","");
    new rule("Cayman Islands","1","011","1");
    new rule("Central African Republic","236","00","");
    new rule("Chad","235","00","");
    new rule("Chile","56","1230", "0", "01");// - Entel
    new rule("Chile","56","1230", "0", "10");// - Entel
    new rule("Chile","56","00","0");
    new rule("China","86","00","0");

    new rule("Colombia","57","009","0", "123"); //  - Movistar
    new rule("Colombia","57","00414","0", "103"); //  - Tigo
    new rule("Colombia","57","00414","0", "111"); //  - Tigo
    new rule("Colombia","57","00468","0", "130"); //  - Avantel
    //new rule("Colombia","57","00456","0"); //  - Claro Fixed
    new rule("Colombia","57","00444","0","101"); //  - Claro Mobile
    new rule("Colombia","57","00","0");

    new rule("Comoros","269","00","");
    new rule("Congo","242","00","");
    new rule("Cook Islands","682","00","");
    new rule("Costa Rica","506","00","");
    new rule("Croatia","385","00","0");
    new rule("Cuba","53","119","0");
    new rule("Curacao","599","00","0");
    new rule("Cyprus","357","00","");
    new rule("Czech Republic","420","00","");
    new rule("Democratic Republic of Congo","243","00","0");
    new rule("Denmark","45","00","");
    new rule("Diego Garcia","246","00","");
    new rule("Djibouti","253","00","");
    new rule("Dominica","1","011","1");
    new rule("Dominican Republic","1","011","1");
    new rule("East Timor","670","00","");
    new rule("Ecuador","593","00","0");
    new rule("Egypt","20","00","0");
    new rule("El Salvador","503","00","");
    new rule("Equatorial Guinea","240","00","");
    new rule("Eritrea","291","00","0");
    new rule("Estonia","372","00","");
    new rule("Ethiopia","251","00","0");
    new rule("Falkland (Malvinas) Islands","500","00","");
    new rule("Faroe Islands","298","00","");
    new rule("Fiji","679","00","");
    new rule("Finland","358","00, 990, 994, 999","0");
    new rule("France","33","00","0");
    new rule("French Guiana","594","00","0");
    new rule("French Polynesia","689","00","");
    new rule("Gabon","241","00","");
    new rule("Gambia","220","00","");
    new rule("Georgia","995","00","0");
    new rule("Germany","49","00","0");
    new rule("Ghana","233","00","0");
    new rule("Gibraltar","350","00","");
    new rule("Greece","30","00","");
    new rule("Greenland","299","00","");
    new rule("Grenada","1","011","1");
    new rule("Guadeloupe","590","00","0");
    new rule("Guam","1","011","1");
    new rule("Guatemala","502","00","");
    new rule("Guinea","224","00","");
    new rule("Guinea-Bissau","245","00","");
    new rule("Guyana","592","001","");
    new rule("Haiti","509","00","");
    new rule("Honduras","504","00","");
    new rule("Hong Kong","852","001","");
    new rule("Hungary","36","00","06");
    new rule("Iceland","354","00","");
    new rule("India","91","00","0");
    new rule("Indonesia","62","001","0","01"); // , 008 - Indosat
    new rule("Indonesia","62","007","0","07"); //  - Telkom
    new rule("Indonesia","62","007","0","20"); //  - Telkom
    new rule("Indonesia","62","009","0","99"); //  - Bakrie Telecom
    new rule("Indonesia","62","00","0");
    new rule("Inmarsat Satellite","870","00","");
    new rule("Iran","98","00","0");
    new rule("Iraq","964","00","");
    new rule("Ireland","353","00","0");
    new rule("Iridium Satellite","8816/8817","00","");
    new rule("Israel","972","00, 012, 013, 014, 018","0");
    new rule("Italy","39","00","");
    new rule("Ivory Coast","225","00","");
    new rule("Jamaica","1","011","1");
    new rule("Japan","81","010","0");
    new rule("Jordan","962","00","0");
    new rule("Kazakhstan","7","810","8");
    new rule("Kenya","254","000 (006 and 007 to Uganda and Tanzania)","0");
    new rule("Kiribati","686","00","");
    new rule("Kuwait","965","00","");
    new rule("Kyrgyzstan","996","00","0");
    new rule("Laos","856","00","0");
    new rule("Latvia","371","00","");
    new rule("Lebanon","961","00","0");
    new rule("Lesotho","266","00","");
    new rule("Liberia","231","00","");
    new rule("Libya","218","00","0");
    new rule("Liechtenstein","423","00","");
    new rule("Lithuania","370","00","8");
    new rule("Luxembourg","352","00","");
    new rule("Macau","853","00","");
    new rule("Macedonia","389","00","0");
    new rule("Madagascar","261","00","0");
    new rule("Malawi","265","00","");
    new rule("Malaysia","60","00","0");
    new rule("Maldives","960","00","");
    new rule("Mali","223","00","");
    new rule("Malta","356","00","");
    new rule("Marshall Islands","692","011","1");
    new rule("Martinique","596","00","0");
    new rule("Mauritania","222","00","");
    new rule("Mauritius","230","00","");
    new rule("Mayotte","262","00","");
    new rule("Mexico","52","00","01");
    new rule("Micronesia","691","011","1");
    new rule("Moldova","373","00","0");
    new rule("Monaco","377","00","");
    new rule("Mongolia","976","001","0");
    new rule("Montenegro","382","00","0");
    new rule("Montserrat","1","011","1");
    new rule("Morocco","212","00","0");
    new rule("Mozambique","258","00","");
    new rule("Myanmar","95","00","0");
    new rule("Namibia","264","00","0");
    new rule("Nauru","674","00","");
    new rule("Nepal","977","00","0");
    new rule("Netherlands","31","00","0");
    new rule("Netherlands Antilles","599","00","0");
    new rule("New Caledonia","687","00","");
    new rule("New Zealand","64","00","0");
    new rule("Nicaragua","505","00","");
    new rule("Niger","227","00","");
    new rule("Nigeria","234","009","0");
    new rule("Niue","683","00","");
    new rule("Norfolk Island","6723","00","");
    new rule("North Korea","850","99","");
    new rule("Northern Marianas","1","011","1");
    new rule("Norway","47","00","");
    new rule("Oman","968","00","");
    new rule("Pakistan","92","00","0");
    new rule("Palau","680","011","");
    new rule("Palestine","970","00","0");
    new rule("Panama","507","00","");
    new rule("Papua New Guinea","675","00","");
    new rule("Paraguay","595","00","0");
    new rule("Peru","51","00","0");
    new rule("Philippines","63","00","0");
    new rule("Poland","48","00","");
    new rule("Portugal","351","00","");
    new rule("Puerto Rico","1","011","1");
    new rule("Qatar","974","00","");
    new rule("Reunion","262","00","");
    new rule("Romania","40","00","0");
    new rule("Russian Federation","7","810","8");
    new rule("Rwanda","250","00","");
    new rule("Saint Helena","290","00","");
    new rule("Saint Kitts and Nevis","1","011","1");
    new rule("Saint Lucia","1","011","1");
    new rule("Saint Barthelemy","590","00","0");
    new rule("Saint Martin (French part)","590","00","0");
    new rule("Saint Pierre and Miquelon","508","00","");
    new rule("Saint Vincent and the Grenadines","1","011","1");
    new rule("Samoa","685","0","");
    new rule("San Marino","378","00","");
    new rule("Sao Tome and Principe","239","00","");
    new rule("Saudi Arabia","966","00","0");
    new rule("Senegal","221","00","");
    new rule("Serbia","381","00","0");
    new rule("Seychelles","248","00","");
    new rule("Sierra Leone","232","00","0");
    new rule("Singapore","65","001, 008","");
    new rule("Sint Maarten","1","00","1");
    new rule("Slovakia","421","00","0");
    new rule("Slovenia","386","00","0");
    new rule("Solomon Islands","677","00","");
    new rule("Somalia","252","00","");
    new rule("South Africa","27","00","0");
    new rule("South Korea","82","001, 002","0");
    new rule("South Sudan","211","00","");
    new rule("Spain","34","00","");
    new rule("Sri Lanka","94","00","0");
    new rule("Sudan","249","00","0");
    new rule("Suriname","597","00","0");
    new rule("Swaziland","268","00","");
    new rule("Sweden","46","00","0");
    new rule("Switzerland","41","00","0");
    new rule("Syria","963","00","0");
    new rule("Taiwan","886","002","0");
    new rule("Tajikistan","992","810","8");
    new rule("Tanzania","255","000","0");
    new rule("Thailand","66","001","0");
    new rule("Thuraya Satellite","882 16","00","");
    new rule("Togo","228","00","");
    new rule("Tokelau","690","00","");
    new rule("Tonga","676","00","");
    new rule("Trinidad and Tobago","1","011","1");
    new rule("Tunisia","216","00","");
    new rule("Turkey","90","00","0");
    new rule("Turkmenistan","993","810","8");
    new rule("Turks and Caicos Islands","1","0","1");
    new rule("Tuvalu","688","00","");
    new rule("Uganda","256","000","0");
    new rule("Ukraine","380","00","0");
    new rule("United Arab Emirates","971","00","0");
    new rule("United Kingdom","44","00","0");
    new rule("United States of America","1","011","1");
    new rule("U.S. Virgin Islands","1","011","1");
    new rule("Uruguay","598","00","0");
    new rule("Uzbekistan","998","810","8");
    new rule("Vanuatu","678","00","");
    new rule("Vatican City","379, 39","00","");
    new rule("Venezuela","58","00","0");
    new rule("Vietnam","84","00","0");
    new rule("Wallis and Futuna","681","00","");
    new rule("Yemen","967","00","0");
    new rule("Zambia","260","00","0");
    new rule("Zimbabwe","263","00","0");
}


QList<rule*> rule::rules;

rule::rule(QString country_name, QString country_prefix, QString exit_code, QString trunk_code, QString network_code, QObject *parent)
    :QObject(parent)
{

    this->country_name = country_name;
    this->country_prefix = country_prefix;
    this->exit_code = exit_code;
    this->trunk_code = trunk_code;
    this->network_code = network_code;

    rules.append(this);
}


rule* rule::find(QString country_prefix, QString mnc) {
    foreach(rule *r, rules) {
        if(r->country_prefix == country_prefix &&
          (r->network_code == mnc || r->network_code == "")) {
            qDebug() << "match!" << r->country_name;
            return r;
        }
    }

    return NULL;
}



QString phonenumber::mobilecc_to_iso32662(int code) {

    QMap<int, QString> mccList;

    mccList.insert(202, "GR");
    mccList.insert(204, "NL");
    mccList.insert(206, "BE");
    mccList.insert(208, "FR");
    mccList.insert(212, "MC");
    mccList.insert(213, "AD");
    mccList.insert(214, "ES");
    mccList.insert(216, "HU");
    mccList.insert(218, "BA");
    mccList.insert(219, "HR");
    mccList.insert(220, "RS");
    mccList.insert(222, "IT");
    mccList.insert(226, "RO");
    mccList.insert(228, "CH");
    mccList.insert(230, "CZ");
    mccList.insert(231, "SK");
    mccList.insert(232, "AT");
    mccList.insert(234, "GB");
    mccList.insert(235, "GB");
    mccList.insert(238, "DK");
    mccList.insert(240, "SE");
    mccList.insert(242, "NO");
    mccList.insert(244, "FI");
    mccList.insert(246, "LT");
    mccList.insert(247, "LV");
    mccList.insert(248, "EE");
    mccList.insert(250, "RU");
    mccList.insert(255, "UA");
    mccList.insert(257, "BY");
    mccList.insert(259, "MD");
    mccList.insert(260, "PL");
    mccList.insert(262, "DE");
    mccList.insert(266, "GI");
    mccList.insert(268, "PT");
    mccList.insert(270, "LU");
    mccList.insert(272, "IE");
    mccList.insert(274, "IS");
    mccList.insert(276, "AL");
    mccList.insert(278, "MT");
    mccList.insert(280, "CY");
    mccList.insert(282, "GE");
    mccList.insert(283, "AM");
    mccList.insert(284, "BG");
    mccList.insert(286, "TR");
    mccList.insert(288, "FO");
    mccList.insert(289, "GE");
    mccList.insert(290, "GL");
    mccList.insert(292, "SM");
    mccList.insert(293, "SI");
    mccList.insert(294, "MK");
    mccList.insert(295, "LI");
    mccList.insert(297, "ME");
    mccList.insert(302, "CA");
    mccList.insert(308, "PM");
    mccList.insert(310, "GU");
    mccList.insert(310, "US");
    mccList.insert(311, "GU");
    mccList.insert(311, "US");
    mccList.insert(312, "US");
    mccList.insert(316, "US");
    mccList.insert(330, "PR");
    mccList.insert(334, "MX");
    mccList.insert(338, "JM");
    mccList.insert(340, "FG");
    mccList.insert(340, "GP");
    mccList.insert(340, "MQ");
    mccList.insert(342, "BB");
    mccList.insert(344, "AG");
    mccList.insert(346, "KY");
    mccList.insert(348, "VG");
    mccList.insert(350, "BM");
    mccList.insert(352, "GD");
    mccList.insert(354, "MS");
    mccList.insert(356, "KN");
    mccList.insert(358, "LC");
    mccList.insert(360, "VC");
    mccList.insert(362, "AN");
    mccList.insert(362, "CW");
    mccList.insert(363, "AW");
    mccList.insert(364, "BS");
    mccList.insert(365, "AI");
    mccList.insert(366, "DM");
    mccList.insert(368, "CU");
    mccList.insert(370, "DO");
    mccList.insert(372, "HT");
    mccList.insert(374, "TT");
    mccList.insert(376, "TC");
    mccList.insert(376, "VI");
    mccList.insert(400, "AZ");
    mccList.insert(401, "KZ");
    mccList.insert(402, "BT");
    mccList.insert(404, "IN");
    mccList.insert(405, "IN");
    mccList.insert(410, "PK");
    mccList.insert(412, "AF");
    mccList.insert(413, "LK");
    mccList.insert(414, "MM");
    mccList.insert(415, "LB");
    mccList.insert(416, "JO");
    mccList.insert(417, "SY");
    mccList.insert(418, "IQ");
    mccList.insert(419, "KW");
    mccList.insert(420, "SA");
    mccList.insert(421, "YE");
    mccList.insert(422, "OM");
    mccList.insert(424, "AE");
    mccList.insert(425, "IL");
    mccList.insert(425, "PS");
    mccList.insert(426, "BH");
    mccList.insert(427, "QA");
    mccList.insert(428, "MN");
    mccList.insert(429, "NP");
    mccList.insert(430, "AE");
    mccList.insert(431, "AE");
    mccList.insert(432, "IR");
    mccList.insert(434, "UZ");
    mccList.insert(436, "TK");
    mccList.insert(437, "KG");
    mccList.insert(438, "TM");
    mccList.insert(440, "JP");
    mccList.insert(441, "JP");
    mccList.insert(450, "KR");
    mccList.insert(452, "VN");
    mccList.insert(454, "HK");
    mccList.insert(455, "MO");
    mccList.insert(456, "KH");
    mccList.insert(457, "LA");
    mccList.insert(460, "CN");
    mccList.insert(466, "TW");
    mccList.insert(467, "KP");
    mccList.insert(470, "BD");
    mccList.insert(472, "MV");
    mccList.insert(502, "MY");
    mccList.insert(505, "AU");
    mccList.insert(510, "ID");
    mccList.insert(514, "TP");
    mccList.insert(515, "PH");
    mccList.insert(520, "TH");
    mccList.insert(525, "SG");
    mccList.insert(528, "BN");
    mccList.insert(530, "NZ");
    mccList.insert(537, "PG");
    mccList.insert(539, "TO");
    mccList.insert(540, "SB");
    mccList.insert(541, "VU");
    mccList.insert(542, "FJ");
    mccList.insert(544, "AS");
    mccList.insert(545, "KI");
    mccList.insert(546, "NC");
    mccList.insert(547, "PF");
    mccList.insert(548, "CK");
    mccList.insert(549, "WS");
    mccList.insert(550, "FM");
    mccList.insert(552, "PW");
    mccList.insert(553, "TV");
    mccList.insert(555, "NU");
    mccList.insert(602, "EG");
    mccList.insert(603, "DZ");
    mccList.insert(604, "MA");
    mccList.insert(605, "TN");
    mccList.insert(606, "LY");
    mccList.insert(607, "GM");
    mccList.insert(608, "SN");
    mccList.insert(609, "MR");
    mccList.insert(610, "ML");
    mccList.insert(611, "GN");
    mccList.insert(612, "CI");
    mccList.insert(613, "BF");
    mccList.insert(614, "NE");
    mccList.insert(615, "TG");
    mccList.insert(616, "BJ");
    mccList.insert(617, "MU");
    mccList.insert(618, "LR");
    mccList.insert(619, "SL");
    mccList.insert(620, "GH");
    mccList.insert(621, "NG");
    mccList.insert(622, "TD");
    mccList.insert(623, "CF");
    mccList.insert(624, "CM");
    mccList.insert(625, "CV");
    mccList.insert(626, "ST");
    mccList.insert(627, "GQ");
    mccList.insert(628, "GA");
    mccList.insert(629, "CG");
    mccList.insert(630, "CD");
    mccList.insert(631, "AO");
    mccList.insert(632, "GW");
    mccList.insert(633, "SC");
    mccList.insert(634, "SD");
    mccList.insert(635, "RW");
    mccList.insert(636, "ET");
    mccList.insert(637, "SO");
    mccList.insert(638, "DJ");
    mccList.insert(639, "KE");
    mccList.insert(640, "TZ");
    mccList.insert(641, "UG");
    mccList.insert(642, "BI");
    mccList.insert(643, "MZ");
    mccList.insert(645, "ZM");
    mccList.insert(646, "MG");
    mccList.insert(647, "RE");
    mccList.insert(648, "ZW");
    mccList.insert(649, "NA");
    mccList.insert(650, "MW");
    mccList.insert(651, "LS");
    mccList.insert(652, "BW");
    mccList.insert(653, "SZ");
    mccList.insert(654, "KM");
    mccList.insert(655, "ZA");
    mccList.insert(657, "ER");
    mccList.insert(659, "SS");
    mccList.insert(702, "BZ");
    mccList.insert(704, "GT");
    mccList.insert(706, "SV");
    mccList.insert(708, "HN");
    mccList.insert(710, "NI");
    mccList.insert(712, "CR");
    mccList.insert(714, "PA");
    mccList.insert(716, "PE");
    mccList.insert(722, "AR");
    mccList.insert(724, "BR");
    mccList.insert(730, "CL");
    mccList.insert(732, "CO");
    mccList.insert(734, "VE");
    mccList.insert(736, "BO");
    mccList.insert(738, "GY");
    mccList.insert(740, "EC");
    mccList.insert(744, "PY");
    mccList.insert(746, "SR");
    mccList.insert(748, "UY");
    mccList.insert(750, "FK");


    if(mccList.contains(code))
        return mccList[code];

    return "";

}

QString phonenumber::countryPrefix(QString country, QString prefix, QString number) {
    typedef QPair<QString,QString> pz;
    QList<pz> c;

    c.append(pz("AF", "93"));
    c.append(pz("AL", "355"));
    c.append(pz("DZ", "213"));
    c.append(pz("AS", "1684"));
    c.append(pz("AD", "376"));
    c.append(pz("AO", "244"));
    c.append(pz("AI", "1264"));

    c.append(pz("AG", "1268"));
    c.append(pz("AR", "54"));
    c.append(pz("AM", "374"));
    c.append(pz("AW", "297"));

    c.append(pz("AU", "61"));
    c.append(pz("CX", "61"));
    c.append(pz("CC", "61"));

    c.append(pz("AT", "43"));
    c.append(pz("AZ", "994"));
    c.append(pz("BS", "1242"));
    c.append(pz("BH", "973"));
    c.append(pz("BD", "880"));
    c.append(pz("BB", "1246"));
    c.append(pz("BY", "375"));
    c.append(pz("BE", "32"));
    c.append(pz("BZ", "501"));
    c.append(pz("BJ", "229"));
    c.append(pz("BM", "1441"));
    c.append(pz("BT", "975"));
    c.append(pz("BO", "591"));
    c.append(pz("BA", "387"));
    c.append(pz("BW", "267"));
    c.append(pz("BR", "55"));
    c.append(pz("IO", "246"));
    c.append(pz("BN", "673"));
    c.append(pz("BG", "359"));
    c.append(pz("BF", "226"));
    c.append(pz("BI", "257"));
    c.append(pz("KH", "855"));
    c.append(pz("CM", "237"));
    c.append(pz("CV", "238"));
    c.append(pz("KY", "1345"));
    c.append(pz("CF", "236"));
    c.append(pz("TD", "235"));
    c.append(pz("CL", "56"));
    c.append(pz("CN", "86"));
    c.append(pz("CO", "57"));
    c.append(pz("KM", "269"));
    c.append(pz("CG", "242"));
    c.append(pz("CD", "243"));
    c.append(pz("CK", "682"));
    c.append(pz("CR", "506"));
    c.append(pz("HR", "385"));
    c.append(pz("CU", "53"));

    c.append(pz("CW", "599"));
    c.append(pz("BQ", "599"));

    c.append(pz("CY", "357"));
    c.append(pz("CZ", "420"));
    c.append(pz("CI", "225"));
    c.append(pz("DK", "45"));
    c.append(pz("DJ", "253"));
    c.append(pz("DM", "1767"));
    c.append(pz("DO", "1809"));
    c.append(pz("DO", "1829"));
    c.append(pz("DO", "1849"));
    c.append(pz("EC", "593"));
    c.append(pz("EG", "20"));
    c.append(pz("SV", "503"));
    c.append(pz("GQ", "240"));
    c.append(pz("ER", "291"));
    c.append(pz("EE", "372"));
    c.append(pz("ET", "251"));

    c.append(pz("FK", "500"));
    c.append(pz("GS", "500"));

    c.append(pz("FO", "298"));
    c.append(pz("FJ", "679"));

    c.append(pz("FI", "358"));
    c.append(pz("AX", "358"));

    c.append(pz("FR", "33"));
    c.append(pz("GF", "594"));
    c.append(pz("PF", "689"));

    c.append(pz("GA", "241"));
    c.append(pz("GM", "220"));
    c.append(pz("GE", "995"));
    c.append(pz("DE", "49"));
    c.append(pz("GH", "233"));
    c.append(pz("GI", "350"));
    c.append(pz("GR", "30"));
    c.append(pz("GL", "299"));
    c.append(pz("GD", "1473"));

    c.append(pz("GP", "590"));
    c.append(pz("BL", "590"));
    c.append(pz("MF", "590"));

    c.append(pz("GU", "1671"));
    c.append(pz("GT", "502"));
    c.append(pz("GN", "224"));
    c.append(pz("GW", "245"));
    c.append(pz("GY", "592"));
    c.append(pz("HT", "509"));
    c.append(pz("VA", "3906"));
    c.append(pz("HN", "504"));
    c.append(pz("HK", "852"));
    c.append(pz("HU", "36"));
    c.append(pz("IS", "354"));
    c.append(pz("IN", "91"));
    c.append(pz("ID", "62"));
    c.append(pz("IR", "98"));
    c.append(pz("IQ", "964"));
    c.append(pz("IE", "353"));
    c.append(pz("IL", "972"));
    c.append(pz("IT", "39"));
    c.append(pz("JM", "1876"));
    c.append(pz("JP", "81"));
    c.append(pz("JO", "962"));
    c.append(pz("KE", "254"));
    c.append(pz("KI", "686"));
    c.append(pz("KP", "850"));
    c.append(pz("KR", "82"));
    c.append(pz("KW", "965"));
    c.append(pz("KG", "996"));
    c.append(pz("LA", "856"));
    c.append(pz("LV", "371"));
    c.append(pz("LB", "961"));
    c.append(pz("LS", "266"));
    c.append(pz("LR", "231"));
    c.append(pz("LY", "218"));
    c.append(pz("LI", "423"));
    c.append(pz("LT", "370"));
    c.append(pz("LU", "352"));
    c.append(pz("MO", "853"));
    c.append(pz("MK", "389"));
    c.append(pz("MG", "261"));
    c.append(pz("MW", "265"));
    c.append(pz("MY", "60"));
    c.append(pz("MV", "960"));
    c.append(pz("ML", "223"));
    c.append(pz("MT", "356"));
    c.append(pz("MH", "692"));
    c.append(pz("MQ", "596"));
    c.append(pz("MR", "222"));
    c.append(pz("MU", "230"));

    c.append(pz("YT", "262"));
    c.append(pz("TF", "262"));
    c.append(pz("RE", "262"));

    c.append(pz("MX", "52"));
    c.append(pz("FM", "691"));
    c.append(pz("MD", "373"));
    c.append(pz("MC", "377"));
    c.append(pz("MN", "976"));
    c.append(pz("ME", "382"));
    c.append(pz("MS", "1664"));

    c.append(pz("MA", "212"));
    c.append(pz("EH", "212"));

    c.append(pz("MZ", "258"));
    c.append(pz("MM", "95"));
    c.append(pz("NA", "264"));
    c.append(pz("NR", "674"));
    c.append(pz("NP", "977"));
    c.append(pz("NL", "31"));
    c.append(pz("NC", "687"));
    c.append(pz("NZ", "64"));
    c.append(pz("NI", "505"));
    c.append(pz("NE", "227"));
    c.append(pz("NG", "234"));
    c.append(pz("NU", "683"));

    c.append(pz("NF", "672"));
    c.append(pz("AQ", "672"));
    c.append(pz("HM", "672"));

    c.append(pz("MP", "1670"));

    c.append(pz("NO", "47"));
    c.append(pz("SJ", "47"));
    c.append(pz("BV", "47"));

    c.append(pz("OM", "968"));
    c.append(pz("PK", "92"));
    c.append(pz("PW", "680"));
    c.append(pz("PS", "970"));
    c.append(pz("PA", "507"));
    c.append(pz("PG", "675"));
    c.append(pz("PY", "595"));
    c.append(pz("PE", "51"));
    c.append(pz("PH", "63"));
    c.append(pz("PN", "870"));
    c.append(pz("PL", "48"));
    c.append(pz("PT", "351"));


    c.append(pz("QA", "974"));
    c.append(pz("RO", "40"));

    c.append(pz("RU", "7"));
    c.append(pz("KZ", "7"));
    c.append(pz("RW", "250"));


    c.append(pz("SH", "290"));
    c.append(pz("KN", "1869"));
    c.append(pz("LC", "1758"));

    c.append(pz("PM", "508"));
    c.append(pz("VC", "1784"));
    c.append(pz("WS", "685"));
    c.append(pz("SM", "378"));
    c.append(pz("ST", "239"));
    c.append(pz("SA", "966"));
    c.append(pz("SN", "221"));
    c.append(pz("RS", "381"));
    c.append(pz("SC", "248"));
    c.append(pz("SL", "232"));
    c.append(pz("SG", "65"));
    c.append(pz("SX", "1721"));
    c.append(pz("SK", "421"));
    c.append(pz("SI", "386"));
    c.append(pz("SB", "677"));
    c.append(pz("SO", "252"));
    c.append(pz("ZA", "27"));

    c.append(pz("SS", "211"));
    c.append(pz("ES", "34"));
    c.append(pz("LK", "94"));
    c.append(pz("SD", "249"));
    c.append(pz("SR", "597"));

    c.append(pz("SZ", "268"));
    c.append(pz("SE", "46"));
    c.append(pz("CH", "41"));
    c.append(pz("SY", "963"));
    c.append(pz("TW", "886"));
    c.append(pz("TJ", "992"));
    c.append(pz("TZ", "255"));
    c.append(pz("TH", "66"));
    c.append(pz("TL", "670"));
    c.append(pz("TG", "228"));
    c.append(pz("TK", "690"));
    c.append(pz("TO", "676"));
    c.append(pz("TT", "1868"));
    c.append(pz("TN", "216"));
    c.append(pz("TR", "90"));
    c.append(pz("TM", "993"));
    c.append(pz("TC", "1649"));
    c.append(pz("TV", "688"));
    c.append(pz("UG", "256"));
    c.append(pz("UA", "380"));
    c.append(pz("AE", "971"));

    c.append(pz("GB", "44"));
    c.append(pz("GG", "44"));
    c.append(pz("IM", "44"));
    c.append(pz("JE", "44"));

    c.append(pz("US", "1"));
    c.append(pz("CA", "1"));
    c.append(pz("PR", "1"));

    c.append(pz("UM", "2"));
    c.append(pz("UY", "598"));
    c.append(pz("UZ", "998"));
    c.append(pz("VU", "678"));
    c.append(pz("VE", "58"));
    c.append(pz("VN", "84"));
    c.append(pz("VG", "1284"));
    c.append(pz("VI", "1340"));
    c.append(pz("WF", "681"));
    c.append(pz("YE", "967"));
    c.append(pz("ZM", "260"));
    c.append(pz("ZW", "263"));




    if(country != "") {
        for(auto _it = c.begin(); _it != c.end(); ++_it) {
            if((*_it).first == country)
                return (*_it).second;
        }
        return "";
    }

    if(prefix != "") {
        for(auto _it = c.begin(); _it != c.end(); ++_it) {
            if((*_it).second == prefix) {
                return (*_it).first;
            }
        }
        return "";
    }

    if(number != "") {
        pz bestCandidate;

        for(auto _it = c.begin(); _it != c.end(); ++_it) {

            if(number.startsWith( (*_it).second ) && (*_it).second.length() > bestCandidate.second.length() ) {
                bestCandidate = *_it;
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
