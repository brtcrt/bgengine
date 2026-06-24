cmake_minimum_required(VERSION 3.25)

project(bgengine
    VERSION 0.1.0
    DESCRIPTION "Shader and media desktop background engine for KDE Plasma"
    LANGUAGES CXX
)

set(BGENGINE_ID "org.brtcrt.bgengine" CACHE STRING "KDE plugin id")
set(BGENGINE_DISPLAY_NAME "BgEngine" CACHE STRING "Human-readable product name")

set(QT_MIN_VERSION "6.11.0")
set(KF_MIN_VERSION "6.27.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    DBus
    Gui
    Qml
    Quick
    ShaderTools
)

qt_standard_project_setup(REQUIRES ${QT_MIN_VERSION})

add_subdirectory(src)
add_subdirectory(wallpapers)