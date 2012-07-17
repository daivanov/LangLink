TEMPLATE = subdirs

OTHER_FILES += \
    langlink.svg \
    langlink.desktop

launcher.files = langlink.desktop
launcher.path = /usr/share/applications
INSTALLS += launcher

contains(MEEGO_EDITION,harmattan) {
    icons.files = langlink.svg
    icons.path = /usr/share/themes/base/meegotouch/icons/
    INSTALLS += icons
}
