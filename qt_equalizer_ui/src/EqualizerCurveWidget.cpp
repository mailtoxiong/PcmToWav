#include "EqualizerCurveWidget.h"

#include <QMouseEvent>
#include <QEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPointF>
#include <QStyle>
#include <QStyleOption>
#include <QLineF>

#include <QtGlobal>

#include <limits>

namespace
{
    constexpr qreal CurveMargin = 16.0;
    constexpr qreal PointRadius = 6.0;
    constexpr qreal HoverDistance = 12.0;
}

EqualizerCurveWidget::EqualizerCurveWidget(QWidget *parent)
    : QWidget(parent)
    , m_bandValues(BandCount, 0)
    , m_minGain(-12)
    , m_maxGain(12)
    , m_activeBand(-1)
    , m_isDragging(false)
{
    setMouseTracking(true);
}

int EqualizerCurveWidget::minimumGain() const
{
    return m_minGain;
}

void EqualizerCurveWidget::setMinimumGain(int minimumGain)
{
    if (m_minGain == minimumGain) {
        return;
    }

    m_minGain = minimumGain;
    if (m_maxGain <= m_minGain) {
        m_maxGain = m_minGain + 1;
    }

    for (int &value : m_bandValues) {
        value = qBound(m_minGain, value, m_maxGain);
    }

    update();
}

int EqualizerCurveWidget::maximumGain() const
{
    return m_maxGain;
}

void EqualizerCurveWidget::setMaximumGain(int maximumGain)
{
    if (m_maxGain == maximumGain) {
        return;
    }

    m_maxGain = maximumGain;
    if (m_maxGain <= m_minGain) {
        m_minGain = m_maxGain - 1;
    }

    for (int &value : m_bandValues) {
        value = qBound(m_minGain, value, m_maxGain);
    }

    update();
}

void EqualizerCurveWidget::setBandValues(const QVector<int> &values)
{
    if (m_bandValues.size() != BandCount) {
        m_bandValues.resize(BandCount);
    }

    for (int i = 0; i < BandCount; ++i) {
        const int requested = i < values.size() ? values.at(i) : 0;
        m_bandValues[i] = qBound(m_minGain, requested, m_maxGain);
    }

    update();
}

QVector<int> EqualizerCurveWidget::bandValues() const
{
    return m_bandValues;
}

void EqualizerCurveWidget::setBandValue(int index, int value)
{
    if (index < 0 || index >= m_bandValues.size()) {
        return;
    }

    const int clamped = qBound(m_minGain, value, m_maxGain);
    if (m_bandValues.at(index) == clamped) {
        return;
    }

    m_bandValues[index] = clamped;
    update();
}

void EqualizerCurveWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QStyleOption opt;
    opt.init(this);

    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    const QRectF rect = curveRect();
    if (!rect.isValid() || rect.width() <= 0.0 || rect.height() <= 0.0) {
        return;
    }

    painter.setRenderHint(QPainter::Antialiasing, true);

    QColor gridColor = palette().mid().color();
    gridColor.setAlpha(90);

    painter.setPen(QPen(gridColor, 1.0, Qt::DashLine));

    for (int value : {m_minGain, 0, m_maxGain}) {
        if (value < m_minGain || value > m_maxGain) {
            continue;
        }
        const qreal y = yForValue(value);
        painter.drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }

    painter.setPen(QPen(gridColor, 1.0));
    painter.drawRect(rect);

    if (m_bandValues.isEmpty()) {
        return;
    }

    QPainterPath path;
    for (int i = 0; i < m_bandValues.size(); ++i) {
        const QPointF pos = bandPosition(i);
        if (i == 0) {
            path.moveTo(pos);
        } else {
            path.lineTo(pos);
        }
    }

    QColor curveColor = palette().highlight().color();
    if (!isEnabled()) {
        curveColor = palette().mid().color();
    }
    curveColor.setAlpha(isEnabled() ? 220 : 150);

    painter.setPen(QPen(curveColor, 2.0));
    painter.drawPath(path);

    QColor pointFill = curveColor;
    pointFill.setAlpha(255);
    QColor pointOutline = palette().dark().color();
    pointOutline.setAlpha(isEnabled() ? 220 : 120);

    painter.setPen(QPen(pointOutline, 1.2));
    painter.setBrush(pointFill);

    for (int i = 0; i < m_bandValues.size(); ++i) {
        const QPointF pos = bandPosition(i);
        painter.drawEllipse(pos, PointRadius, PointRadius);
    }
}

void EqualizerCurveWidget::mousePressEvent(QMouseEvent *event)
{
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

    const QPointF pos = event->pos();
    int closestBand = -1;
    qreal closestDistance = std::numeric_limits<qreal>::max();

    for (int i = 0; i < m_bandValues.size(); ++i) {
        const qreal distance = QLineF(pos, bandPosition(i)).length();
        if (distance <= HoverDistance && distance < closestDistance) {
            closestDistance = distance;
            closestBand = i;
        }
    }

    if (closestBand >= 0) {
        m_activeBand = closestBand;
        m_isDragging = true;
        setCursor(Qt::ClosedHandCursor);
        setBandValueFromUser(m_activeBand, valueForY(pos.y()));
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void EqualizerCurveWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging && m_activeBand >= 0) {
        setBandValueFromUser(m_activeBand, valueForY(event->pos().y()));
        event->accept();
        return;
    }

    updateHoverCursor(event->pos());
    QWidget::mouseMoveEvent(event);
}

void EqualizerCurveWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isDragging && event->button() == Qt::LeftButton) {
        m_isDragging = false;
        m_activeBand = -1;
        unsetCursor();
        updateHoverCursor(event->pos());
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}

void EqualizerCurveWidget::leaveEvent(QEvent *event)
{
    if (!m_isDragging) {
        m_activeBand = -1;
        unsetCursor();
    }

    QWidget::leaveEvent(event);
}

QRectF EqualizerCurveWidget::curveRect() const
{
    QRectF rect(0.0, 0.0, width(), height());
    rect.adjust(CurveMargin, CurveMargin, -CurveMargin, -CurveMargin);
    return rect;
}

QPointF EqualizerCurveWidget::bandPosition(int index) const
{
    const QRectF rect = curveRect();
    if (!rect.isValid() || BandCount <= 1) {
        return rect.center();
    }

    const qreal step = rect.width() / static_cast<qreal>(BandCount - 1);
    const qreal x = rect.left() + step * index;
    const int value = index < m_bandValues.size() ? m_bandValues.at(index) : 0;
    const qreal y = yForValue(value);

    return QPointF(x, y);
}

void EqualizerCurveWidget::setBandValueFromUser(int index, int value)
{
    if (index < 0 || index >= m_bandValues.size()) {
        return;
    }

    const int clamped = qBound(m_minGain, value, m_maxGain);
    if (m_bandValues.at(index) == clamped) {
        return;
    }

    m_bandValues[index] = clamped;
    update();
    emit bandValueChanged(index, clamped);
}

int EqualizerCurveWidget::valueForY(qreal y) const
{
    const QRectF rect = curveRect();
    if (!rect.isValid()) {
        return 0;
    }

    const qreal clampedY = qBound(rect.top(), y, rect.bottom());
    const qreal range = m_maxGain - m_minGain;
    if (qFuzzyIsNull(range)) {
        return m_minGain;
    }

    const qreal ratio = (rect.bottom() - clampedY) / rect.height();
    const qreal value = m_minGain + ratio * range;

    return qRound(value);
}

qreal EqualizerCurveWidget::yForValue(int value) const
{
    const QRectF rect = curveRect();
    if (!rect.isValid()) {
        return 0.0;
    }

    const int clamped = qBound(m_minGain, value, m_maxGain);
    const qreal range = m_maxGain - m_minGain;
    if (qFuzzyIsNull(range)) {
        return rect.center().y();
    }

    const qreal ratio = (static_cast<qreal>(clamped) - m_minGain) / range;
    return rect.bottom() - ratio * rect.height();
}

void EqualizerCurveWidget::updateHoverCursor(const QPoint &pos)
{
    if (m_isDragging) {
        setCursor(Qt::ClosedHandCursor);
        return;
    }

    if (!isEnabled()) {
        unsetCursor();
        return;
    }

    int closestBand = -1;
    qreal closestDistance = HoverDistance + 1.0;

    for (int i = 0; i < m_bandValues.size(); ++i) {
        const qreal distance = QLineF(pos, bandPosition(i)).length();
        if (distance < closestDistance) {
            closestDistance = distance;
            closestBand = i;
        }
    }

    if (closestBand >= 0 && closestDistance <= HoverDistance) {
        setCursor(Qt::OpenHandCursor);
    } else {
        unsetCursor();
    }
}
