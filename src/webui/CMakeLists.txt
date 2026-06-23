# Build setup — guarded so it only runs once whether reached via add_subdirectory or include()
if(NOT TARGET RNBOUIData)
  set(WEBUI_DIST_HTML "${CMAKE_CURRENT_LIST_DIR}/dist/index.html")
  set(WEBUI_DIST_JS   "${CMAKE_CURRENT_LIST_DIR}/dist/index.js")

  set(WEBUI_SOURCES
      "${CMAKE_CURRENT_LIST_DIR}/index.html"
      "${CMAKE_CURRENT_LIST_DIR}/main.js"
      "${CMAKE_CURRENT_LIST_DIR}/vite.config.js"
      "${CMAKE_CURRENT_LIST_DIR}/package.json"
  )

  find_program(NPM_EXECUTABLE npm REQUIRED)

  add_custom_command(
      OUTPUT  ${WEBUI_DIST_HTML} ${WEBUI_DIST_JS}
      COMMAND ${NPM_EXECUTABLE} install
      COMMAND ${NPM_EXECUTABLE} run build
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      DEPENDS ${WEBUI_SOURCES}
      COMMENT "Building web UI (npm run build)"
      VERBATIM
  )

  add_custom_target(RNBOWebUIBuild
      DEPENDS ${WEBUI_DIST_HTML} ${WEBUI_DIST_JS}
  )

  juce_add_binary_data(RNBOUIData
      NAMESPACE RNBOUIData
      SOURCES
      ${WEBUI_DIST_HTML}
      ${WEBUI_DIST_JS}
  )

  add_dependencies(RNBOUIData RNBOWebUIBuild)
endif()

# Target configuration — only runs when include()'d from App.cmake or Plugin.cmake
if(DEFINED RNBO_TARGET)
  target_sources(${RNBO_TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/WebBrowserAudioEditor.cpp)
  target_compile_definitions(${RNBO_TARGET} PRIVATE RNBO_EDITOR_WEBVIEW)
  target_compile_definitions(${RNBO_TARGET} PUBLIC JUCE_WEB_BROWSER=1)
  target_link_libraries(${RNBO_TARGET} PRIVATE RNBOUIData)
endif()
