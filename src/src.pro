TEMPLATE = app
TARGET = langlink
DEPENDPATH += .
INCLUDEPATH += .

CONFIG += debug
QT += core webkit gui network

SOURCES += main.cpp
HEADERS += translator.h
SOURCES += translator.cpp
HEADERS += translation-handler.h
SOURCES += translation-handler.cpp
HEADERS += link-view.h
SOURCES += link-view.cpp
HEADERS += link-item.h
SOURCES += link-item.cpp
HEADERS += link-button.h
SOURCES += link-button.cpp
