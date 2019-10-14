TARGET = biotool2
TEMPLATE = app

QT += opengl xml widgets

GIT_VERSION = $$system(git rev-parse --short HEAD)
DEFINES += BT1_GIT_VERSION=$$GIT_VERSION

QMAKE_CXXFLAGS += -std=gnu++0x

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/imageView.cpp \
    src/dataCtrl.cpp \
    src/point.cpp
HEADERS += src/mainwindow.h \
    src/imageView.h \
    src/dataCtrl.h \
    src/point.h

INCLUDEPATH += src

OTHER_FILES += README.md

FORMS += ui/mainwindow.ui

RESOURCES += res/icons.qrc
