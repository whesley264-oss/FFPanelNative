cmake_minimum_required(VERSION 3.22.1)

project("ffpanelnative")

add_library(
    ffpanel
    SHARED
    native-lib.cpp
    renderer.cpp
    ui_components.cpp
)

find_library(
    log-lib
    log
)

target_link_libraries(
    ffpanel
    android
    log
    EGL
    GLESv3
    ${log-lib}
)
