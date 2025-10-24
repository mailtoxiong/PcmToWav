QT += widgets

CONFIG += c++11

TEMPLATE = app
TARGET = equalizer-ui

SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/EqualizerWidget.cpp \
    src/PresetManager.cpp \
    src/EqualizerCurveWidget.cpp

HEADERS += \
    src/MainWindow.h \
    src/EqualizerWidget.h \
    src/PresetManager.h \
    src/EqualizerCurveWidget.h

FORMS += \
    ui/MainWindow.ui \
    ui/EqualizerWidget.ui

RESOURCES += resources.qrc
