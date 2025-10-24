#include "EqualizerWidget.h"
#include "ui_EqualizerWidget.h"
#include "EqualizerCurveWidget.h"

#include <QLabel>
#include <QSlider>
#include <QtGlobal>
#include <QVariant>

EqualizerWidget::EqualizerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EqualizerWidget)
    , m_curveWidget(nullptr)
    , m_isBypassed(false)
{
    ui->setupUi(this);

    initializeBands();
    initializeCurve();
}

EqualizerWidget::~EqualizerWidget()
{
    delete ui;
}

void EqualizerWidget::setBandValues(const QVector<int> &values)
{
    const int count = m_bands.size();
    for (int i = 0; i < count; ++i) {
        const int requested = i < values.size() ? values.at(i) : 0;
        const int clamped = qBound(m_bands.at(i).slider->minimum(), requested, m_bands.at(i).slider->maximum());
        QSlider *slider = m_bands.at(i).slider;
        const bool blocked = slider->blockSignals(true);
        slider->setValue(clamped);
        slider->blockSignals(blocked);
        updateValueLabel(i, clamped);
    }

    if (m_curveWidget) {
        m_curveWidget->setBandValues(bandValues());
    }
}

QVector<int> EqualizerWidget::bandValues() const
{
    QVector<int> values;
    values.reserve(m_bands.size());
    for (const BandControl &band : m_bands) {
        values.append(band.slider->value());
    }
    return values;
}

void EqualizerWidget::resetBands()
{
    QVector<int> flatValues(m_bands.size(), 0);
    setBandValues(flatValues);
}

void EqualizerWidget::setBypassed(bool bypassed)
{
    if (m_isBypassed == bypassed) {
        return;
    }

    m_isBypassed = bypassed;
    applyBypassState();
}

bool EqualizerWidget::isBypassed() const
{
    return m_isBypassed;
}

void EqualizerWidget::handleSliderValueChanged(int value)
{
    QSlider *slider = qobject_cast<QSlider *>(sender());
    if (!slider) {
        return;
    }

    const QVariant indexVariant = slider->property("bandIndex");
    if (!indexVariant.isValid()) {
        return;
    }

    const int bandIndex = indexVariant.toInt();
    if (bandIndex < 0 || bandIndex >= m_bands.size()) {
        return;
    }

    updateValueLabel(bandIndex, value);

    if (m_curveWidget) {
        m_curveWidget->setBandValue(bandIndex, value);
    }

    emit bandValueChanged(bandIndex, value);
}

void EqualizerWidget::initializeBands()
{
    m_bands.clear();
    m_bands.reserve(BandCount);

    const QVector<QSlider *> sliders = {
        ui->sliderBand0,
        ui->sliderBand1,
        ui->sliderBand2,
        ui->sliderBand3,
        ui->sliderBand4,
        ui->sliderBand5,
        ui->sliderBand6,
        ui->sliderBand7,
        ui->sliderBand8,
        ui->sliderBand9
    };

    const QVector<QLabel *> valueLabels = {
        ui->labelValue0,
        ui->labelValue1,
        ui->labelValue2,
        ui->labelValue3,
        ui->labelValue4,
        ui->labelValue5,
        ui->labelValue6,
        ui->labelValue7,
        ui->labelValue8,
        ui->labelValue9
    };

    for (int i = 0; i < BandCount; ++i) {
        QSlider *slider = sliders.value(i, nullptr);
        QLabel *valueLabel = valueLabels.value(i, nullptr);

        if (!slider || !valueLabel) {
            continue;
        }

        slider->setOrientation(Qt::Vertical);
        slider->setMinimum(-12);
        slider->setMaximum(12);
        slider->setSingleStep(1);
        slider->setPageStep(1);
        slider->setTickInterval(3);
        slider->setTickPosition(QSlider::TicksBothSides);
        slider->setValue(0);
        slider->setProperty("bandIndex", i);

        connect(slider, &QSlider::valueChanged, this, &EqualizerWidget::handleSliderValueChanged);

        BandControl control;
        control.slider = slider;
        control.valueLabel = valueLabel;
        m_bands.append(control);

        const int bandIndex = m_bands.size() - 1;
        updateValueLabel(bandIndex, slider->value());
    }
}

void EqualizerWidget::initializeCurve()
{
    m_curveWidget = ui->curveWidget;
    if (!m_curveWidget) {
        return;
    }

    int minimumGain = -12;
    int maximumGain = 12;

    if (!m_bands.isEmpty() && m_bands.first().slider) {
        minimumGain = m_bands.first().slider->minimum();
        maximumGain = m_bands.first().slider->maximum();
    }

    m_curveWidget->setMinimumGain(minimumGain);
    m_curveWidget->setMaximumGain(maximumGain);
    m_curveWidget->setBandValues(bandValues());
    m_curveWidget->setEnabled(!m_isBypassed);

    connect(m_curveWidget, &EqualizerCurveWidget::bandValueChanged, this, &EqualizerWidget::handleCurveBandValueChanged);
}

void EqualizerWidget::updateValueLabel(int bandIndex, int value)
{
    if (bandIndex < 0 || bandIndex >= m_bands.size()) {
        return;
    }

    QLabel *label = m_bands.at(bandIndex).valueLabel;
    if (!label) {
        return;
    }

    QString text = QString::number(value);
    if (value > 0) {
        text.prepend(QLatin1Char('+'));
    }
    text.append(QStringLiteral(" dB"));
    label->setText(text);
}

void EqualizerWidget::handleCurveBandValueChanged(int bandIndex, int value)
{
    if (bandIndex < 0 || bandIndex >= m_bands.size()) {
        return;
    }

    QSlider *slider = m_bands.at(bandIndex).slider;
    if (!slider) {
        return;
    }

    slider->setValue(value);
}

void EqualizerWidget::applyBypassState()
{
    for (int i = 0; i < m_bands.size(); ++i) {
        if (QSlider *slider = m_bands.at(i).slider) {
            slider->setEnabled(!m_isBypassed);
        }
        if (QLabel *label = m_bands.at(i).valueLabel) {
            label->setEnabled(!m_isBypassed);
        }
    }

    if (m_curveWidget) {
        m_curveWidget->setEnabled(!m_isBypassed);
    }
}
