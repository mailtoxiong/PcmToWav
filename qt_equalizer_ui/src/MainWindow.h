#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "PresetManager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void handlePresetChanged(int index);
    void handleResetClicked();
    void handleBypassToggled(bool checked);
    void handleBandValueChanged(int bandIndex, int value);

private:
    Ui::MainWindow *ui;
    PresetManager m_presetManager;

    void initializeUi();
    void applyPreset(const QString &presetName);
    void updateStatusIndicator(const QString &presetName);
};

#endif // MAINWINDOW_H
