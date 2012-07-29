include(translation.pri)

TEMPLATE = subdirs

QMFILES = $$replace(TRANSLATIONS, .ts, .qm)

translations.files = $$QMFILES
translations.path = /opt/langlink/translation/
INSTALLS += translations
