#ifndef EQUALIZERCURVEWIDGET_H
#define EQUALIZERCURVEWIDGET_H

#include <QVector>
#include <QWidget>
#include <QRectF>
#include <QPointF>

class EqualizerCurveWidget : public QWidget
{
    Q_OBJECT

public:
    static const int BandCount = 10;

    explicit EqualizerCurveWidget(QWidget *parent = nullptr);

    int minimumGain() const;
    void setMinimumGain(int minimumGain);

    int maximumGain() const;
    void setMaximumGain(int maximumGain);

    void setBandValues(const QVector<int> &values);
    QVector<int> bandValues() const;
    void setBandValue(int index, int value);

signals:
    void bandValueChanged(int bandIndex, int value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QVector<int> m_bandValues;
    int m_minGain;
    int m_maxGain;
    int m_activeBand;
    bool m_isDragging;

    QRectF curveRect() const;
    QPointF bandPosition(int index) const;
    void setBandValueFromUser(int index, int value);
    int valueForY(qreal y) const;
    qreal yForValue(int value) const;
    void updateHoverCursor(const QPoint &pos);
};

#endif // EQUALIZERCURVEWIDGET_H
