CONFIG += ordered
TEMPLATE = subdirs
SUBDIRS = src translation data fonts

OTHER_FILES += \
    debian/rules \
    debian/manifest.aegis \
    debian/copyright \
    debian/control \
    debian/compat \
    debian/changelog \
    debian/langlink.install
