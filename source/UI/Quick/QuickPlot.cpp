#include "QuickPlot.h"
#include <QChart>
#include <QXYSeries>
#include <QAbstractBarSeries>
#include <QBarSet>
#include <QValueAxis>
#include <QPainter>
#include <QPainterPath>
#include <QHoverEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QLocale>
#include <limits>
QT_CHARTS_USE_NAMESPACE

QuickPlot::QuickPlot(QQuickItem *parent):QQuickPaintedItem(parent) {
    setAntialiasing(true); setAcceptHoverEvents(true); setAcceptedMouseButtons(Qt::LeftButton);
    connect(this,&QuickPlot::colorsChanged,this,[this]{update();});
    // The source chart can change series while a stats filter is being applied.
    auto *timer=new QTimer(this); timer->setInterval(300);
    connect(timer,&QTimer::timeout,this,[this]{ if(isVisible()) update(); }); timer->start();
}
void QuickPlot::setChart(QObject *chart) { if(m_chart==chart) return; m_chart=chart; resetZoom(); emit chartChanged(); update(); }
void QuickPlot::resetZoom() { m_left=0; m_right=1; update(); }
void QuickPlot::paint(QPainter *p) {
    auto *chart=qobject_cast<QChart *>(m_chart); if(!chart || width()<100 || height()<100) return;
    p->setRenderHint(QPainter::Antialiasing);
    const QRectF area(58,40,qMax(1.,width()-76),qMax(1.,height()-82));
    QFont font; font.setPointSizeF(10); p->setFont(font); p->setPen(m_ink);
    p->drawText(QRectF(8,4,width()-16,24),Qt::AlignLeft|Qt::AlignVCenter,chart->title());
    qreal x0=std::numeric_limits<qreal>::max(),x1=-x0,y0=x0,y1=-x0;
    bool bars=false;
    for(auto *abstract:chart->series()) {
        if(auto *series=qobject_cast<QXYSeries *>(abstract)) for(const auto &point:series->pointsVector()) {
            if(!qIsFinite(point.x()) || !qIsFinite(point.y())) continue;
            x0=qMin(x0,point.x());x1=qMax(x1,point.x());y0=qMin(y0,point.y());y1=qMax(y1,point.y());
        }
        if(auto *series=qobject_cast<QAbstractBarSeries *>(abstract)) {
            bars=true; x0=0; x1=qMax(1,series->barSets().size()); y0=0;
            for(auto *set:series->barSets()) for(int i=0;i<set->count();++i) y1=qMax(y1,set->at(i));
        }
    }
    if(x1<x0) { p->drawText(area,Qt::AlignCenter,tr("Brak danych dla wybranych filtrów")); return; }
    if(qFuzzyCompare(x0,x1)) x1=x0+1;
    if(qFuzzyCompare(y0,y1)) y1=y0+1;
    bool reverse=false;
    for(auto *axis:chart->axes(Qt::Vertical)) if(auto *value=qobject_cast<QValueAxis *>(axis)) { y0=value->min(); y1=qMax(y0+.001,value->max()); reverse=axis->isReverse(); }
    const qreal full=x1-x0, start=x0+full*m_left, end=x0+full*m_right;
    auto mapped=[&](QPointF v) { return QPointF(area.left()+(v.x()-start)/(end-start)*area.width(),area.bottom()-(reverse?1-(v.y()-y0)/(y1-y0):(v.y()-y0)/(y1-y0))*area.height()); };
    for(int i=0;i<=4;++i) {
        const qreal y=area.top()+area.height()*i/4.;
        p->setPen(QPen(m_line,1)); p->drawLine(QPointF(area.left(),y),QPointF(area.right(),y));
        p->setPen(m_ink); const qreal value=reverse?y0+(y1-y0)*i/4.:y1-(y1-y0)*i/4.;
        p->drawText(QRectF(0,y-10,50,20),Qt::AlignRight|Qt::AlignVCenter,QLocale().toString(value,'f',value==int(value)?0:1));
        const qreal x=area.left()+area.width()*i/4.;
        p->drawText(QRectF(x-40,area.bottom()+9,80,20),Qt::AlignCenter,QLocale().toString(start+(end-start)*i/4.,'f',0));
    }
    p->save();p->setClipRect(area.adjusted(-3,-3,3,3));
    int seriesIndex=0;
    for(auto *abstract:chart->series()) {
        QColor color=m_accent; if(seriesIndex++) color=m_ink;
        if(auto *series=qobject_cast<QXYSeries *>(abstract)) {
            QPainterPath path; bool first=true;
            const auto points=series->pointsVector();
            for(const auto &point:points) { const auto v=mapped(point); if(first){path.moveTo(v);first=false;}else path.lineTo(v); }
            p->setPen(QPen(color,2.5));p->setBrush(Qt::NoBrush);p->drawPath(path);
            if(m_point>=0 && m_point<points.size()) { p->setBrush(color);p->drawEllipse(mapped(points[m_point]),5,5); }
        }
        if(auto *series=qobject_cast<QAbstractBarSeries *>(abstract)) {
            int i=0;for(auto *set:series->barSets()) {
                const auto top=mapped(QPointF(i+.18,set->count()?set->at(0):0));
                const auto bottom=mapped(QPointF(i+.82,0));
                p->setBrush(color);p->setPen(Qt::NoPen);p->drawRoundedRect(QRectF(top,bottom).normalized(),3,3);++i;
            }
        }
    }
    p->restore();
    if(bars) for(auto *abstract:chart->series()) if(auto *series=qobject_cast<QAbstractBarSeries *>(abstract)) {
        int i=0; for(auto *set:series->barSets()) { const qreal x=mapped(QPointF(i+.5,0)).x();p->setPen(m_ink);p->drawText(QRectF(x-48,area.bottom()+25,96,20),Qt::AlignCenter,set->label());++i; }
    }
}
void QuickPlot::inspectX(qreal x) {
    auto *chart=qobject_cast<QChart *>(m_chart);if(!chart)return;
    for(auto *abstract:chart->series()) if(auto *series=qobject_cast<QXYSeries *>(abstract)) {
        const auto points=series->pointsVector();if(points.isEmpty())return;
        const qreal fraction=m_left+qBound(0.,(x-58)/qMax(1.,width()-76),1.)*(m_right-m_left);
        const qreal target=points.first().x()+fraction*(points.last().x()-points.first().x());
        int nearest=0;for(int i=1;i<points.size();++i)if(qAbs(points[i].x()-target)<qAbs(points[nearest].x()-target))nearest=i;
        m_point=nearest; const auto point=points[nearest];
        m_hoverText=tr("%1  ·  %2").arg(QLocale().toString(point.x()),QLocale().toString(point.y()));emit hoverTextChanged();
        emit series->hovered(point,true);update();return;
    }
}
void QuickPlot::hoverMoveEvent(QHoverEvent *event){inspectX(event->posF().x());}
void QuickPlot::wheelEvent(QWheelEvent *event){
    const qreal center=(m_left+m_right)/2,span=qBound(.02,(m_right-m_left)*(event->angleDelta().y()>0?.8:1.25),1.);
    m_left=qBound(0.,center-span/2,1.-span);m_right=m_left+span;update();event->accept();
}
void QuickPlot::mousePressEvent(QMouseEvent *event){m_pressX=event->localPos().x();inspectX(m_pressX);}
void QuickPlot::mouseMoveEvent(QMouseEvent *event){inspectX(event->localPos().x());}
void QuickPlot::mouseReleaseEvent(QMouseEvent *event){
    const qreal x=event->localPos().x();if(m_pressX>=0 && qAbs(x-m_pressX)>15){
        const qreal span=m_right-m_left,left=m_left;
        m_left=left+span*qBound(0.,(qMin(x,m_pressX)-58)/qMax(1.,width()-76),.98);
        m_right=left+span*qBound(.02,(qMax(x,m_pressX)-58)/qMax(1.,width()-76),1.);
        if(m_right-m_left<.02)m_right=qMin(1.,m_left+.02);update();
    }m_pressX=-1;
}
void QuickPlot::inspect(int delta){
    auto *chart=qobject_cast<QChart *>(m_chart);if(!chart)return;
    for(auto *abstract:chart->series())if(auto *series=qobject_cast<QXYSeries *>(abstract)){
        const int count=series->count();if(count<1)return; m_point=qBound(0,m_point+delta,count-1);
        inspectX(58+(count>1?qreal(m_point)/(count-1):0)*qMax(1.,width()-76));return;
    }
}
