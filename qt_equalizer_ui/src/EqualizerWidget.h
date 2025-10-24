#ifndef EQUALIZERWIDGET_H
#define EQUALIZERWIDGET_H

#include <QWidget>
#include <QVector>

#include <QString>

class QLabel;
class QSlider;
class EqualizerCurveWidget;

namespace Ui {
class EqualizerWidget;
}

class EqualizerWidget : public QWidget
{
    Q_OBJECT

public:
    static const int BandCount = 10;

    explicit EqualizerWidget(QWidget *parent = nullptr);
    ~EqualizerWidget() override;

    void setBandValues(const QVector<int> &values);
    QVector<int> bandValues() const;

    void resetBands();

    void setBypassed(bool bypassed);
    bool isBypassed() const;

signals:
    void bandValueChanged(int bandIndex, int value);

private slots:
    void handleSliderValueChanged(int value);
    void handleCurveBandValueChanged(int bandIndex, int value);

private:
    Ui::EqualizerWidget *ui;
    EqualizerCurveWidget *m_curveWidget;

    struct BandControl
    {
        QSlider *slider;
        QLabel *valueLabel;
    };

    QVector<BandControl> m_bands;
    bool m_isBypassed;

    void initializeBands();
    void initializeCurve();
    void updateValueLabel(int bandIndex, int value);
    void applyBypassState();
};

#endif // EQUALIZERWIDGET_H
