#pragma once
#include <QQuickPaintedItem>
#include <QPointer>
#include <QColor>

class QuickPlot : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY(QObject *chart READ chart WRITE setChart NOTIFY chartChanged)
    Q_PROPERTY(QColor ink MEMBER m_ink NOTIFY colorsChanged)
    Q_PROPERTY(QColor line MEMBER m_line NOTIFY colorsChanged)
    Q_PROPERTY(QColor accent MEMBER m_accent NOTIFY colorsChanged)
    Q_PROPERTY(QString hoverText READ hoverText NOTIFY hoverTextChanged)
public:
    explicit QuickPlot(QQuickItem *parent=nullptr);
    QObject *chart() const { return m_chart; }
    void setChart(QObject *chart);
    QString hoverText() const { return m_hoverText; }
    void paint(QPainter *painter) override;
    Q_INVOKABLE void resetZoom();
    Q_INVOKABLE void inspect(int delta);
signals:
    void chartChanged();
    void colorsChanged();
    void hoverTextChanged();
protected:
    void hoverMoveEvent(QHoverEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    void inspectX(qreal x);
    QPointer<QObject> m_chart;
    QColor m_ink, m_line, m_accent;
    QString m_hoverText;
    qreal m_left=0, m_right=1, m_pressX=-1;
    int m_point=-1;
};
