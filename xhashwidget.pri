INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xhashwidget.h \
    $$PWD/dialoghash.h

SOURCES += \
    $$PWD/xhashwidget.cpp \
    $$PWD/dialoghash.cpp

FORMS += \
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
