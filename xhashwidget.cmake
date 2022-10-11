include_directories(${CMAKE_CURRENT_LIST_DIR})

# TODO
# xformats
# xlineedithex
include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/xdialogprocess.cmake)

set(XHASHWIDGET_SOURCES
    ${XDIALOGPROCESS_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialoghash.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialoghash.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialoghashprocess.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialoghashprocess.ui
    ${CMAKE_CURRENT_LIST_DIR}/hashprocess.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xhashwidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xhashwidget.ui
)
