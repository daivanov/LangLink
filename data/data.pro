OTHER_FILES += \
    langlink.svg

TEMPLATE = subdirs

contains(MEEGO_EDITION,harmattan) {
    icons.files = langlink.svg
    icons.path = /usr/share/themes/base/meegotouch/icons/
    INSTALLS += icons
}
