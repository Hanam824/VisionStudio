# ── VisionApp Executable ─────────────────────────────────────────────────────
cmake_minimum_required(VERSION 3.21)

# Gather sources
file(GLOB_RECURSE VISIONAPP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE VISIONAPP_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

set(VISIONAPP_RESOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/app.qrc"
)

# Create executable (WIN32 flag hides console on Windows release builds).
add_executable(VisionApp WIN32
    ${VISIONAPP_SOURCES}
    ${VISIONAPP_HEADERS}
    ${VISIONAPP_RESOURCES}
)

# Include directories.
target_include_directories(VisionApp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link against VisionCore (for headers / interface types only — actual DLL
# is loaded at runtime via QLibrary).  We link statically for the interface
# types but dynamic-load the implementation.
target_link_libraries(VisionApp
    PRIVATE
        VisionCore      # Interface headers + link-time symbols
        Qt5::Widgets
        Qt5::Core
        Qt5::Gui
)

# On Windows, copy VisionCore DLL next to VisionApp after build.
if(WIN32)
    add_custom_command(TARGET VisionApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:VisionCore>
            $<TARGET_FILE_DIR:VisionApp>
        COMMENT "Copying VisionCore.dll to output directory"
    )
endif()

# ── IDE grouping ─────────────────────────────────────────────────────────────
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${VISIONAPP_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"     PREFIX "Source Files" FILES ${VISIONAPP_SOURCES})
source_group("Resources" FILES ${VISIONAPP_RESOURCES})

set_target_properties(VisionApp PROPERTIES
    OUTPUT_NAME "VisionStudio"
)
