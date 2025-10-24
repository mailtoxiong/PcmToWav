#ifndef EQUALIZERWIDGET_H
#define EQUALIZERWIDGET_H

#include <QWidget>
#include <QVector>

#include <QString>

class QLabel;
class QSlider;

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

private:
    Ui::EqualizerWidget *ui;

    struct BandControl
    {
        QSlider *slider;
        QLabel *valueLabel;
    };

    QVector<BandControl> m_bands;
    bool m_isBypassed;

    void initializeBands();
    void updateValueLabel(int bandIndex, int value);
    void applyBypassState();
};

#endif // EQUALIZERWIDGET_H
