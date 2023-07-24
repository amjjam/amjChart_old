QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += debug_and_release c++11
TARGET = amjChart
TEMPLATE = lib
DEFINES += AMJCHART_LIBRARY

VERSION=0.1.0

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    amjChartView.C \
    main.C

HEADERS += \
    amjChart.H

FORMS += \
    amjChartView.ui

# Default rules for deployment.

LIBS += -L/opt/QCustomPlot/lib/ -lqcustomplot
INCLUDEPATH += /opt/QCustomPlot/include
DEPENDPATH += /opt/QCustomPlot/include

target.path=/opt/amjChart/lib
headers.path=/opt/amjChart/include
headers.files=amjChart.H

INSTALLS += target headers

RESOURCES += \
    icons.qrc
