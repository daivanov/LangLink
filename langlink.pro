CONFIG += ordered
TEMPLATE = subdirs
SUBDIRS = src translation data

OTHER_FILES += \
    debian/rules \
    debian/manifest.aegis \
    debian/copyright \
    debian/control \
    debian/compat \
    debian/changelog \
    debian/langlink.install
