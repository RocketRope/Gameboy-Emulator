#-------------------------------------------------
#
# Project created by QtCreator 2019-06-29T21:09:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gbe
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
    Qt/widgets/backgroundmapwidget.cpp \
    Qt/widgets/basegameboywidget.cpp \
    Qt/widgets/cpuregisterswidget.cpp \
    Qt/widgets/cpuflagswidget.cpp \
    Qt/widgets/interruptwidget.cpp \
    Qt/widgets/joypadwidget.cpp \
    Qt/widgets/lcdregisterswidget.cpp \
    Qt/widgets/memorywidget.cpp \
    Qt/widgets/oamwidget.cpp \
    Qt/widgets/objectwidget.cpp \
    Qt/widgets/serialwidget.cpp \
    Qt/widgets/tilegraphicsitem.cpp \
    Qt/widgets/tilemapwidget.cpp \
    Qt/widgets/timerwidget.cpp \
    Qt/windows/basewindow.cpp \
    Qt/windows/debugwindow.cpp \
    Qt/windows/screenwindow.cpp \
    Qt/windows/vramwindow.cpp \
    src/cpu.cpp \
    src/el/easylogging++.cc \
    src/gameboy.cpp \
    src/main.cpp \
    src/cartridge.cpp \
    src/memory.cpp \
    src/timer.cpp \
    src/types.cpp \
    src/video.cpp

HEADERS += \
    Qt/widgets/backgroundmapwidget.h \
    Qt/widgets/basegameboywidget.h \
    Qt/widgets/cpuregisterswidget.h \
    Qt/widgets/cpuflagswidget.h \
    Qt/widgets/interruptwidget.h \
    Qt/widgets/joypadwidget.h \
    Qt/widgets/lcdregisterswidget.h \
    Qt/widgets/memorywidget.h \
    Qt/widgets/oamwidget.h \
    Qt/widgets/objectwidget.h \
    Qt/widgets/serialwidget.h \
    Qt/widgets/tilegraphicsitem.h \
    Qt/widgets/tilemapwidget.h \
    Qt/widgets/timerwidget.h \
    Qt/windows/basewindow.h \
    Qt/windows/debugwindow.h \
    Qt/windows/screenwindow.h \
    Qt/windows/vramwindow.h \
    include/cpu.h \
    include/decoder.h \
    include/el/easylogging++.h \
    include/gameboy.h \
    include/memory.h \
    include/timer.h \
    include/types.h \
    include/cartridge.h \
    include/video.h

FORMS += \
    Qt/widgets/backgroundmapwidget.ui \
    Qt/widgets/cpuregisterswidget.ui \
    Qt/widgets/cpuflagswidget.ui \
    Qt/widgets/interruptwidget.ui \
    Qt/widgets/joypadwidget.ui \
    Qt/widgets/lcdregisterswidget.ui \
    Qt/widgets/memorywidget.ui \
    Qt/widgets/oamwidget.ui \
    Qt/widgets/objectwidget.ui \
    Qt/widgets/serialwidget.ui \
    Qt/widgets/tilemapwidget.ui \
    Qt/widgets/timerwidget.ui \
    Qt/windows/basewindow.ui \
    Qt/windows/debugwindow.ui \
    Qt/windows/screenwindow.ui \
    Qt/windows/vramwindow.ui

INCLUDEPATH = \
    include \
    include/el \
    Qt/windows \
    Qt/widgets

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

