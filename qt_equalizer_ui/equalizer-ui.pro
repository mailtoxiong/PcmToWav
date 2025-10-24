QT += widgets

CONFIG += c++11

TEMPLATE = app
TARGET = equalizer-ui

SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/EqualizerWidget.cpp \
    src/PresetManager.cpp

HEADERS += \
    src/MainWindow.h \
    src/EqualizerWidget.h \
    src/PresetManager.h

FORMS += \
    ui/MainWindow.ui \
    ui/EqualizerWidget.ui

RESOURCES += resources.qrc
