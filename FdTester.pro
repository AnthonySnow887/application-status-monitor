
include(QtSsh/QtSsh.pri)
include(QCustomPlot/QCustomPlot.pri)
#include(ProcessLoad/ProcessLoad.pri)
include(QtSystemLoad/QtSystemLoad.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    TabHostStatistics/CpuHostWidget.cpp \
    TabHostStatistics/DriveHostWidget.cpp \
    TabHostStatistics/MemHostWidget.cpp \
    TabHostStatistics/NetworkHostWidget.cpp \
    TabHostStatistics/TabHostStatistics.cpp \
    TabSockets/SocketsGraphWidget.cpp \
    TabSockets/TabSockets.cpp \
    main.cpp \
    MainWindow.cpp \
    TabGraphics/TabGraphics.cpp \
    TabGraphics/CpuGraphWidget.cpp \
    TabGraphics/FdGraphWidget.cpp \
    TabGraphics/MemGraphWidget.cpp \
    TabGraphics/ThreadsGraphWidget.cpp


HEADERS += \
    BaseTabWidget.h \
    BaseWidget.h \
    CmdInfo.h \
    MainWindow.h \
    TabGraphics/CpuGraphWidget.h \
    TabGraphics/FdGraphWidget.h \
    TabGraphics/MemGraphWidget.h \
    TabGraphics/ThreadsGraphWidget.h \
    TabGraphics/TabGraphics.h \
    TabHostStatistics/CpuHostWidget.h \
    TabHostStatistics/DriveHostWidget.h \
    TabHostStatistics/MemHostWidget.h \
    TabHostStatistics/NetworkHostWidget.h \
    TabHostStatistics/TabHostStatistics.h \
    TabSockets/SocketData.h \
    TabSockets/SocketsGraphWidget.h \
    TabSockets/TabSockets.h


FORMS += \
    MainWindow.ui \
    TabGraphics/TabGraphics.ui \
    TabGraphics/CpuGraphWidget.ui \
    TabGraphics/FdGraphWidget.ui \
    TabGraphics/MemGraphWidget.ui \
    TabGraphics/ThreadsGraphWidget.ui \
    TabHostStatistics/CpuHostWidget.ui \
    TabHostStatistics/DriveHostWidget.ui \
    TabHostStatistics/MemHostWidget.ui \
    TabHostStatistics/NetworkHostWidget.ui \
    TabHostStatistics/TabHostStatistics.ui \
    TabSockets/SocketsGraphWidget.ui \
    TabSockets/TabSockets.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
