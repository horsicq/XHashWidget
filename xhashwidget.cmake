include_directories(${CMAKE_CURRENT_LIST_DIR})

# TODO Check includes
# xformats
# xlineedithex

if (NOT DEFINED XDIALOGPROCESS_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/xdialogprocess.cmake)
    set(XHASHWIDGET_SOURCES ${XHASHWIDGET_SOURCES} ${XDIALOGPROCESS_SOURCES})
endif()

set(XHASHWIDGET_SOURCES
    ${XHASHWIDGET_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialoghash.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialoghash.h
    ${CMAKE_CURRENT_LIST_DIR}/dialoghash.ui
    ${CMAKE_CURRENT_LIST_DIR}/hashprocess.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hashprocess.h
    ${CMAKE_CURRENT_LIST_DIR}/xhashwidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xhashwidget.h
    ${CMAKE_CURRENT_LIST_DIR}/xhashwidget.ui
)
