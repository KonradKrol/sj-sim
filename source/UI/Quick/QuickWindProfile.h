#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QVariantList>
#include <QVariantMap>
#include <QStringList>
#include <algorithm>
#include <cmath>

namespace QuickWindProfile {

// Read recorded exposure only. A newer engine or current weather must never
// change the wind shown for an archived jump. Grouping is length-weighted.
inline QVariantMap fromMetadata(const QJsonObject &metadata)
{
    const auto sections=metadata["exposures"].toArray();
    if(metadata["engineVersion"].toInt()!=2 || sections.isEmpty() || sections.size()>32)return {};
    const QStringList phases{"before","experienced","after"};
    double previousEnd=0;
    for(const auto &section:sections) {
        if(!section.isObject())return {};
        const auto part=section.toObject();
        for(const auto &key:{"begin","end","traversed"})if(!part[key].isDouble() || !std::isfinite(part[key].toDouble()))return {};
        const double begin=part["begin"].toDouble(),end=part["end"].toDouble(),length=part["traversed"].toDouble();
        if(begin<0 || begin<previousEnd-1e-6 || end<begin || end>1e7 || length<0 || length>end-begin+1e-6)return {};
        previousEnd=end;
        for(const auto &phase:phases) {
            const auto vector=part[phase].toArray();if(vector.size()!=2)return {};
            for(const auto &value:vector)if(!value.isDouble() || !std::isfinite(value.toDouble()) || std::abs(value.toDouble())>1000)return {};
        }
    }
    QVariantList samples[3];
    const int groupSize=(sections.size()+5)/6;
    for(int first=0;first<sections.size();first+=groupSize) {
        const int last=std::min(first+groupSize,int(sections.size()));
        const double begin=sections[first].toObject()["begin"].toDouble(),end=sections[last-1].toObject()["end"].toDouble();
        for(int phase=0;phase<3;++phase) {
            double head=0,cross=0,length=0;
            for(int i=first;i<last;++i) {
                const auto part=sections[i].toObject();
                const double weight=phase==1?part["traversed"].toDouble():part["end"].toDouble()-part["begin"].toDouble();
                const auto wind=part[phases[phase]].toArray();
                head+=wind[0].toDouble()*weight;cross+=wind[1].toDouble()*weight;length+=weight;
            }
            if(length>0){head/=length;cross/=length;}
            const double strength=std::hypot(head,cross);
            double direction=strength>1e-9?std::atan2(cross,head)*180/3.14159265358979323846:0;
            if(direction<0)direction+=360;
            samples[phase].append(QVariantMap{{"distance",(begin+end)/2},{"begin",begin},{"end",end},
                {"head",head},{"cross",cross},{"strength",strength},{"direction",direction},{"available",length>0}});
        }
    }
    return {{"before",samples[0]},{"during",samples[1]},{"after",samples[2]}};
}

}
