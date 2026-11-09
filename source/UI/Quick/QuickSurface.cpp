#include "QuickSurface.h"
#include <QPainter>
#include <QPainterPath>

QuickSurface::QuickSurface(QQuickItem *parent):QQuickPaintedItem(parent) {
    setAntialiasing(true);connect(this,&QuickSurface::changed,this,[this]{update();});
}
void QuickSurface::paint(QPainter *painter) {
    const qreal inset=m_stroke*.5+1,w=width()-2*inset,h=height()-2*inset;
    if(w<=0 || h<=0)return;
    const qreal halfEdge=qMin(w,h)/2;
    qreal r=qBound(0.,m_corner,halfEdge),s=qBound(r,m_sweep,halfEdge);
    if(m_themeRoundness>=0) {
        r=qMin(halfEdge*qBound(0.,m_themeRoundness,1.),qMax(0.,m_themeCornerLimit));
        s=r+(qMin(halfEdge,qMax(r,m_themeCornerLimit*2))-r)*qBound(0.,m_themeAsymmetry,1.);
    }
    QPainterPath path;path.moveTo(r,0);path.lineTo(w-s,0);path.quadTo(w,0,w,s);
    path.lineTo(w,h-r);path.quadTo(w,h,w-r,h);path.lineTo(s,h);path.quadTo(0,h,0,h-s);
    path.lineTo(0,r);path.quadTo(0,0,r,0);path.closeSubpath();
    painter->translate(inset,inset);painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(m_fill);painter->setPen(m_stroke>0?QPen(m_edge,m_stroke):QPen(Qt::NoPen));painter->drawPath(path);
}
