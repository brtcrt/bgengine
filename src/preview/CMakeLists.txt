qt_add_executable(bgengine-preview
    main.cpp
)

qt_add_qml_module(bgengine-preview
    URI BgEnginePreview
    VERSION 1.0
    RESOURCE_PREFIX /
    QML_FILES
        qml/Main.qml
)

target_compile_features(bgengine-preview PRIVATE cxx_std_20)

target_link_libraries(bgengine-preview
    PRIVATE
        bgengineengine
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
)

target_compile_definitions(bgengine-preview
    PRIVATE
        BGENGINE_BUILD_IMPORT_PATH="${CMAKE_BINARY_DIR}/src/engine"
)

install(TARGETS bgengine-preview ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
