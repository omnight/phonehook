TEMPLATE = subdirs
SUBDIRS = \
    daemon \
    gui \
    $${NULL}

OTHER_FILES = \
    patch/phonehook-lipstick-v4.patch \
    rpm/phonehook.spec \
    rpm/phonehook.changes.in \
    $${NULL}


patch.files = patch/*.patch
patch.path = /usr/share/phonehook

INSTALLS = patch
