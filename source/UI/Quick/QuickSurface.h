#pragma once
#include <QQuickPaintedItem>
#include <QColor>

class QuickSurface : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY(QColor fillColor MEMBER m_fill NOTIFY changed)
    Q_PROPERTY(QColor edgeColor MEMBER m_edge NOTIFY changed)
    Q_PROPERTY(qreal strokeWidth MEMBER m_stroke NOTIFY changed)
    Q_PROPERTY(qreal corner MEMBER m_corner NOTIFY changed)
    Q_PROPERTY(qreal sweep MEMBER m_sweep NOTIFY changed)
public:
    explicit QuickSurface(QQuickItem *parent=nullptr);
    void paint(QPainter *painter) override;
signals:
    void changed();
private:
    QColor m_fill=Qt::white,m_edge=Qt::transparent;
    qreal m_stroke=0,m_corner=6,m_sweep=24;
};
