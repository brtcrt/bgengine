qt_add_library(bgengineengine SHARED)

qt_add_qml_module(bgengineengine
    URI BgEngine
    VERSION 1.0
    RESOURCE_PREFIX /
    OUTPUT_DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/BgEngine"
    SOURCES
        bgenginecontroller.cpp
        bgenginecontroller.h
    QML_FILES
        qml/BgEngineView.qml
        qml/MediaLayer.qml
        qml/NoiseTexture.qml
        qml/ShaderSurface.qml
)

qt_add_shaders(bgengineengine bgengine_shaders
    PREFIX
        "/bgengine/shaders"
    BASE
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    BATCHABLE
    FILES
        shaders/fullscreen.vert
        shaders/shadertoy.frag
        shaders/postprocess.frag
)

target_compile_features(bgengineengine PUBLIC cxx_std_20)

target_link_libraries(bgengineengine
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
)

set_target_properties(bgengineengineplugin PROPERTIES
    PREFIX "lib"
    BUILD_RPATH "$ORIGIN"
    INSTALL_RPATH "$ORIGIN"
)

set_target_properties(bgengineengine PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/BgEngine"
)

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/BgEngine/"
    DESTINATION "${KDE_INSTALL_QMLDIR}/BgEngine"
    PATTERN "*.a" EXCLUDE
    PATTERN "*.so" EXCLUDE
    PATTERN "*.qrc" EXCLUDE
    PATTERN "bgengineengineplugin.so" EXCLUDE
)

install(TARGETS bgengineengine bgengineengineplugin
    LIBRARY DESTINATION "${KDE_INSTALL_QMLDIR}/BgEngine"
)
