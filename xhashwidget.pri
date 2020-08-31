INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialoghashprocess.h \
    $$PWD/hashprocess.h \
    $$PWD/xhashwidget.h \
    $$PWD/dialoghash.h

SOURCES += \
    $$PWD/dialoghashprocess.cpp \
    $$PWD/hashprocess.cpp \
    $$PWD/xhashwidget.cpp \
    $$PWD/dialoghash.cpp

FORMS += \
    $$PWD/dialoghashprocess.ui \
    $$PWD/xhashwidget.ui \
    $$PWD/dialoghash.ui

!contains(XCONFIG, xformats) {
    XCONFIG += xformats
    include($$PWD/../Formats/xformats.pri)
}

!contains(XCONFIG, xlineedithex) {
    XCONFIG += xlineedithex
    include($$PWD/../Controls/xlineedithex.pri)
}
