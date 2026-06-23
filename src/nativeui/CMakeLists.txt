target_sources(${RNBO_TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CustomAudioEditor.cpp)
target_compile_definitions(${RNBO_TARGET} PRIVATE RNBO_EDITOR_NATIVE)
