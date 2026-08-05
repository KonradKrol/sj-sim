#include "CountryFlagsManager.h"

#include <QPixmap>
#include <QFile>
#include <QString>
#include <QIcon>
#include <QDebug>
#include <QSize>

CountryFlagsManager::CountryFlagsManager()
{

}

QPixmap CountryFlagsManager::getFlagPixmap(const QString &twoLettersCountryCode)
{
    QPixmap p("flags/" + twoLettersCountryCode.toUpper() + ".svg");

    QFile f("flags/" + twoLettersCountryCode.toUpper() + ".svg");
    return p;
}

/*QString CountryFlagsManager::convertThreeLettersCountryCodeToTwoLetters(const QString &code)
{
    if(code == "afg") return "af";
    else if(code == "alb") return "al";
    else if(code == "dza") return "dz";
    else if(code == "asm") return "as";
    else if(code == "and") return "ad";
    else if(code == "ago") return "ao";
    else if(code == "aia") return "ai";
    else if(code == "ata") return "aq";
    else if(code == "atg") return "ag";
    else if(code == "arg") return "ar";
    else if(code == "arm") return "am";
    else if(code == "abw") return "aw";
    else if(code == "aus") return "au";
    else if(code == "aut") return "at";
    else if(code == "aze") return "az";
    else if(code == "bhs") return "bs";
    else if(code == "bhr") return "bh";
    else if(code == "bgd") return "bd";
    else if(code == "brb") return "bb";
    else if(code == "blr") return "by";
    else if(code == "bel") return "be";
    else if(code == "blz") return "bz";
    else if(code == "ben") return "bj";
    else if(code == "bmu") return "bm";
    else if(code == "btn") return "bt";
    else if(code == "bol") return "bo";
    else if(code == "bes") return "bq";
    else if(code == "bih") return "ba";
    else if(code == "bwa") return "bw";
    else if(code == "bvt") return "bv";
    else if(code == "bra") return "br";
    else if(code == "iot") return "io";
    else if(code == "brn") return "bn";
    else if(code == "bgr") return "bg";
    else if(code == "bfa") return "bf";
    else if(code == "bdi") return "bi";
    else if(code == "cpv") return "cv";
    else if(code == "khm") return "kh";
    else if(code == "cmr") return "cm";
    else if(code == "can") return "ca";
    else if(code == "cym") return "ky";
    else if(code == "caf") return "cf";
    else if(code == "tcd") return "td";
    else if(code == "chl") return "cl";
    else if(code == "chn") return "cn";
    else if(code == "cxr") return "cx";
    else if(code == "cck") return "cc";
    else if(code == "col") return "co";
    else if(code == "com") return "km";
    else if(code == "cod") return "cd";
    else if(code == "cog") return "cg";
    else if(code == "cok") return "ck";
    else if(code == "cri") return "cr";
    else if(code == "hrv") return "hr";
    else if(code == "cub") return "cu";
    else if(code == "cuw") return "cw";
    else if(code == "cyp") return "cy";
    else if(code == "cze") return "cz";
    else if(code == "civ") return "ci";
    else if(code == "dnk") return "dk";
    else if(code == "dji") return "dj";
    else if(code == "dma") return "dm";
    else if(code == "dom") return "do";
    else if(code == "ecu") return "ec";
    else if(code == "egy") return "eg";
    else if(code == "slv") return "sv";
    else if(code == "gnq") return "gq";
    else if(code == "eri") return "er";
    else if(code == "est") return "ee";
    else if(code == "swz") return "sz";
    else if(code == "eth") return "et";
    else if(code == "flk") return "fk";
    else if(code == "fro") return "fo";
    else if(code == "fji") return "fj";
    else if(code == "fin") return "fi";
    else if(code == "fra") return "fr";
    else if(code == "guf") return "gf";
    else if(code == "pyf") return "pf";
    else if(code == "atf") return "tf";
    else if(code == "gab") return "ga";
    else if(code == "gmb") return "gm";
    else if(code == "geo") return "ge";
    else if(code == "deu") return "de";
    else if(code == "gha") return "gh";
    else if(code == "gib") return "gi";
    else if(code == "grc") return "gr";
    else if(code == "grl") return "gl";
    else if(code == "grd") return "gd";
    else if(code == "glp") return "gp";
    else if(code == "gum") return "gu";
    else if(code == "gtm") return "gt";
    else if(code == "ggy") return "gg";
    else if(code == "gin") return "gn";
    else if(code == "gnb") return "gw";
    else if(code == "guy") return "gy";
    else if(code == "hti") return "ht";
    else if(code == "hmd") return "hm";
    else if(code == "vat") return "va";
    else if(code == "hnd") return "hn";
    else if(code == "hkg") return "hk";
    else if(code == "hun") return "hu";
    else if(code == "isl") return "is";
    else if(code == "ind") return "in";
    else if(code == "idn") return "id";
    else if(code == "irn") return "ir";
    else if(code == "irq") return "iq";
    else if(code == "irl") return "ie";
    else if(code == "imn") return "im";
    else if(code == "isr") return "il";
    else if(code == "ita") return "it";
    else if(code == "jam") return "jm";
    else if(code == "jpn") return "jp";
    else if(code == "jey") return "je";
    else if(code == "jor") return "jo";
    else if(code == "kaz") return "kz";
    else if(code == "ken") return "ke";
    else if(code == "kir") return "ki";
    else if(code == "prk") return "kp";
    else if(code == "kor") return "kr";
    else if(code == "kwt") return "kw";
    else if(code == "kgz") return "kg";
    else if(code == "lao") return "la";
    else if(code == "lva") return "lv";
    else if(code == "lbn") return "lb";
    else if(code == "lso") return "ls";
    else if(code == "lbr") return "lr";
    else if(code == "lby") return "ly";
    else if(code == "lie") return "li";
    else if(code == "ltu") return "lt";
    else if(code == "lux") return "lu";
    else if(code == "mac") return "mo";
    else if(code == "mdg") return "mg";
    else if(code == "mwi") return "mw";
    else if(code == "mys") return "my";
    else if(code == "mdv") return "mv";
    else if(code == "mli") return "ml";
    else if(code == "mlt") return "mt";
    else if(code == "mhl") return "mh";
    else if(code == "mtq") return "mq";
    else if(code == "mrt") return "mr";
    else if(code == "mus") return "mu";
    else if(code == "myt") return "yt";
    else if(code == "mex") return "mx";
    else if(code == "fsm") return "fm";
    else if(code == "mda") return "md";
    else if(code == "mco") return "mc";
    else if(code == "mng") return "mn";
    else if(code == "mne") return "me";
    else if(code == "msr") return "ms";
    else if(code == "mar") return "ma";
    else if(code == "moz") return "mz";
    else if(code == "mmr") return "mm";
    else if(code == "nam") return "na";
    else if(code == "nru") return "nr";
    else if(code == "npl") return "np";
    else if(code == "nld") return "nl";
    else if(code == "ncl") return "nc";
    else if(code == "nzl") return "nz";
    else if(code == "nic") return "ni";
    else if(code == "ner") return "ne";
    else if(code == "nga") return "ng";
    else if(code == "niu") return "nu";
    else if(code == "nfk") return "nf";
    else if(code == "mnp") return "mp";
    else if(code == "nor") return "no";
    else if(code == "omn") return "om";
    else if(code == "pak") return "pk";
    else if(code == "plw") return "pw";
    else if(code == "pse") return "ps";
    else if(code == "pan") return "pa";
    else if(code == "png") return "pg";
    else if(code == "pry") return "py";
    else if(code == "per") return "pe";
    else if(code == "phl") return "ph";
    else if(code == "pcn") return "pn";
    else if(code == "pol") return "pl";
    else if(code == "prt") return "pt";
    else if(code == "pri") return "pr";
    else if(code == "qat") return "qa";
    else if(code == "mkd") return "mk";
    else if(code == "rou") return "ro";
    else if(code == "rus") return "ru";
    else if(code == "rwa") return "rw";
    else if(code == "reu") return "re";
    else if(code == "blm") return "bl";
    else if(code == "shn") return "sh";
    else if(code == "kna") return "kn";
    else if(code == "lca") return "lc";
    else if(code == "maf") return "mf";
    else if(code == "spm") return "pm";
    else if(code == "vct") return "vc";
    else if(code == "wsm") return "ws";
    else if(code == "smr") return "sm";
    else if(code == "stp") return "st";
    else if(code == "sau") return "sa";
    else if(code == "sen") return "sn";
    else if(code == "srb") return "rs";
    else if(code == "syc") return "sc";
    else if(code == "sle") return "sl";
    else if(code == "sgp") return "sg";
    else if(code == "sxm") return "sx";
    else if(code == "svk") return "sk";
    else if(code == "svn") return "si";
    else if(code == "slb") return "sb";
    else if(code == "som") return "so";
    else if(code == "zaf") return "za";
    else if(code == "sgs") return "gs";
    else if(code == "ssd") return "ss";
    else if(code == "esp") return "es";
    else if(code == "lka") return "lk";
    else if(code == "sdn") return "sd";
    else if(code == "sur") return "sr";
    else if(code == "sjm") return "sj";
    else if(code == "swe") return "se";
    else if(code == "che") return "ch";
    else if(code == "syr") return "sy";
    else if(code == "twn") return "tw";
    else if(code == "tjk") return "tj";
    else if(code == "tza") return "tz";
    else if(code == "tha") return "th";
    else if(code == "tls") return "tl";
    else if(code == "tgo") return "tg";
    else if(code == "tkl") return "tk";
    else if(code == "ton") return "to";
    else if(code == "tto") return "tt";
    else if(code == "tun") return "tn";
    else if(code == "tur") return "tr";
    else if(code == "tkm") return "tm";
    else if(code == "tca") return "tc";
    else if(code == "tuv") return "tv";
    else if(code == "uga") return "ug";
    else if(code == "ukr") return "ua";
    else if(code == "are") return "ae";
    else if(code == "gbr") return "gb";
    else if(code == "umi") return "um";
    else if(code == "usa") return "us";
    else if(code == "ury") return "uy";
    else if(code == "uzb") return "uz";
    else if(code == "vut") return "vu";
    else if(code == "ven") return "ve";
    else if(code == "vnm") return "vn";
    else if(code == "vgb") return "vg";
    else if(code == "vir") return "vi";
    else if(code == "wlf") return "wf";
    else if(code == "esh") return "eh";
    else if(code == "yem") return "ye";
    else if(code == "zmb") return "zm";
    else if(code == "zwe") return "zw";
    else if(code == "ala") return "ax";
    else return "xx";
}*/

QSize CountryFlagsManager::getFlagPixmapSize(short type)
{
    switch(type)
    {
    case Normal:
        return QSize(38, 22);
    }
    return QSize(0, 0);
}
