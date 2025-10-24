#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "EqualizerWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QStatusBar>
#include <QStringList>
#include <QVector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initializeUi();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handlePresetChanged(int index)
{
    if (index < 0) {
        return;
    }

    const QString presetName = ui->presetComboBox->itemText(index);
    applyPreset(presetName);
}

void MainWindow::handleResetClicked()
{
    ui->equalizerWidget->resetBands();

    const int flatIndex = ui->presetComboBox->findText(QStringLiteral("Flat"), Qt::MatchFixedString);
    if (flatIndex >= 0) {
        QSignalBlocker blocker(ui->presetComboBox);
        ui->presetComboBox->setCurrentIndex(flatIndex);
    }

    updateStatusIndicator(QStringLiteral("Flat"));
}

void MainWindow::handleBypassToggled(bool checked)
{
    ui->equalizerWidget->setBypassed(checked);

    if (statusBar()) {
        const QString message = checked ? tr("Equalizer bypassed") : tr("Equalizer active");
        statusBar()->showMessage(message, 2000);
    }
}

void MainWindow::handleBandValueChanged(int /*bandIndex*/, int /*value*/)
{
    const QVector<int> currentValues = ui->equalizerWidget->bandValues();
    const QStringList presets = m_presetManager.presetNames();

    for (const QString &presetName : presets) {
        if (m_presetManager.presetValues(presetName) == currentValues) {
            const int index = ui->presetComboBox->findText(presetName, Qt::MatchFixedString);
            if (index >= 0) {
                QSignalBlocker blocker(ui->presetComboBox);
                ui->presetComboBox->setCurrentIndex(index);
            }
            updateStatusIndicator(presetName);
            return;
        }
    }

    if (statusBar()) {
        statusBar()->showMessage(tr("Custom equalizer settings"), 1500);
    }
}

void MainWindow::initializeUi()
{
    const QStringList presets = m_presetManager.presetNames();
    ui->presetComboBox->clear();
    ui->presetComboBox->addItems(presets);

    const int flatIndex = ui->presetComboBox->findText(QStringLiteral("Flat"), Qt::MatchFixedString);
    if (flatIndex >= 0) {
        ui->presetComboBox->setCurrentIndex(flatIndex);
        applyPreset(QStringLiteral("Flat"));
    } else if (!presets.isEmpty()) {
        ui->presetComboBox->setCurrentIndex(0);
        applyPreset(ui->presetComboBox->currentText());
    }

    connect(ui->presetComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::handlePresetChanged);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::handleResetClicked);
    connect(ui->bypassCheckBox, &QCheckBox::toggled, this, &MainWindow::handleBypassToggled);
    connect(ui->equalizerWidget, &EqualizerWidget::bandValueChanged, this, &MainWindow::handleBandValueChanged);

    updateStatusIndicator(ui->presetComboBox->currentText());
}

void MainWindow::applyPreset(const QString &presetName)
{
    const QVector<int> values = m_presetManager.presetValues(presetName);
    ui->equalizerWidget->setBandValues(values);
    updateStatusIndicator(presetName);
}

void MainWindow::updateStatusIndicator(const QString &presetName)
{
    if (!statusBar()) {
        return;
    }

    statusBar()->showMessage(tr("Preset: %1").arg(presetName), 2000);
}
