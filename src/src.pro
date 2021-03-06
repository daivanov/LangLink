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
HEADERS += link-progress-indicator.h
SOURCES += link-progress-indicator.cpp

include(../translation/translation.pri)

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else {
        exists($$[QT_INSTALL_BINS]/lrelease) {
            QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
        } else {
            QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease-qt4
        }
    }
}

buildqm.CONFIG += no_link target_predeps
buildqm.input = TRANSLATIONS
buildqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
buildqm.commands = $$QMAKE_LRELEASE -idbased ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
QMAKE_EXTRA_COMPILERS += buildqm

target.path = /opt/langlink/bin/
INSTALLS += target
