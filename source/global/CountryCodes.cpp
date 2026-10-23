#include "CountryCodes.h"
#include <QHash>

namespace {
const QHash<QString,QString> &fisCodes() {
    // FIS National Ski and Snowboard Associations, checked 2026-10-23.
    // https://www.fis-ski.com/DB/v2/national-ski-and-snowboard-associations
    static const QHash<QString,QString> codes{
        {"DZA","ALG"},{"ASM","ASA"},{"BHS","BAH"},{"BRB","BAR"},{"BMU","BER"},
        {"BTN","BHU"},{"BGR","BUL"},{"CYM","CAY"},{"CHL","CHI"},{"CRI","CRC"},
        {"HRV","CRO"},{"DNK","DEN"},{"SLV","ESA"},{"FJI","FIJ"},{"GNB","GBS"},
        {"DEU","GER"},{"GRC","GRE"},{"GRD","GRN"},{"GTM","GUA"},{"HTI","HAI"},
        {"HND","HON"},{"IDN","INA"},{"IRN","IRI"},{"VIR","ISV"},{"VGB","IVB"},
        {"XKX","KOS"},{"SAU","KSA"},{"KWT","KUW"},{"LVA","LAT"},{"LSO","LES"},
        {"MDG","MAD"},{"MYS","MAS"},{"MNG","MGL"},{"MCO","MON"},{"NLD","NED"},
        {"NPL","NEP"},{"NGA","NGR"},{"PRY","PAR"},{"PHL","PHI"},{"PSE","PLE"},
        {"PRT","POR"},{"PRI","PUR"},{"ZAF","RSA"},{"SVN","SLO"},{"LKA","SRI"},
        {"SDN","SUD"},{"CHE","SUI"},{"TZA","TAN"},{"TON","TGA"},{"TGO","TOG"},
        {"TWN","TPE"},{"ARE","UAE"},{"URY","URU"},{"VUT","VAN"},{"ZWE","ZIM"}
    };
    return codes;
}
}

QString CountryCodes::toFis(const QString &code) {
    const auto normalized=code.trimmed().toUpper();
    return fisCodes().value(normalized,normalized);
}

QString CountryCodes::toIso(const QString &code) {
    const auto normalized=code.trimmed().toUpper();
    static const QHash<QString,QString> aliases=[] {
        QHash<QString,QString> result;
        for(auto it=fisCodes().cbegin();it!=fisCodes().cend();++it)result.insert(it.value(),it.key());
        return result;
    }();
    return aliases.value(normalized,normalized);
}
