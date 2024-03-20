FILE(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

foreach(SOURCE_FILE ${SOURCE_FILES})
    get_filename_component(TOOL_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${TOOL_NAME} ${SOURCE_FILE})
    target_link_libraries(${TOOL_NAME} PRIVATE ${CORE_LIBRARY})
endforeach()
