configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    DESTINATION ${KDE_INSTALL_DATADIR}/plasma/wallpapers/${BGENGINE_ID}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/contents
    DESTINATION ${KDE_INSTALL_DATADIR}/plasma/wallpapers/${BGENGINE_ID}
)
