CONFIG += ordered
TEMPLATE = subdirs
SUBDIRS = src

OTHER_FILES += \
    debian/rules \
    debian/manifest.aegis \
    debian/copyright \
    debian/control \
    debian/compat \
    debian/changelog \
    debian/langlink.install

OTHER_FILES += \
    data/langlink.svg

