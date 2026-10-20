#include "QuickSurface.h"
#include <QPainter>
#include <QPainterPath>

QuickSurface::QuickSurface(QQuickItem *parent):QQuickPaintedItem(parent) {
    setAntialiasing(true);connect(this,&QuickSurface::changed,this,[this]{update();});
}
void QuickSurface::paint(QPainter *painter) {
    const qreal inset=m_stroke*.5+1,w=width()-2*inset,h=height()-2*inset;
    if(w<=0 || h<=0)return;
    const qreal r=qBound(0.,m_corner,qMin(w,h)/2),s=qBound(r,m_sweep,qMin(w,h)/2);
    QPainterPath path;path.moveTo(r,0);path.lineTo(w-s,0);path.quadTo(w,0,w,s);
    path.lineTo(w,h-r);path.quadTo(w,h,w-r,h);path.lineTo(s,h);path.quadTo(0,h,0,h-s);
    path.lineTo(0,r);path.quadTo(0,0,r,0);path.closeSubpath();
    painter->translate(inset,inset);painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(m_fill);painter->setPen(m_stroke>0?QPen(m_edge,m_stroke):QPen(Qt::NoPen));painter->drawPath(path);
}
