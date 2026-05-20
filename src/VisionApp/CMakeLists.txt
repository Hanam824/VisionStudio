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

# On Windows, generate version resource metadata
if(WIN32)
    string(REPLACE "." "," RC_VERSION_COMMA "${PROJECT_VERSION}")
    set(RC_VERSION_COMMA "${RC_VERSION_COMMA},0")

    set(RC_FILE_TYPE "0x1L") # VFT_APP
    set(RC_COMPANY_NAME "VisionStudio Team")
    set(RC_FILE_DESCRIPTION "Cross-platform OCR & Image Processing Studio")
    set(RC_INTERNAL_NAME "VisionStudio")
    set(RC_COPYRIGHT "Copyright (c) 2026 VisionStudio Team. All rights reserved.")
    set(RC_ORIGINAL_FILENAME "VisionStudio.exe")
    set(RC_PRODUCT_NAME "VisionStudio")

    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/version.rc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/VisionApp.rc"
        @ONLY
    )
    target_sources(VisionApp PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/VisionApp.rc")
endif()

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
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
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

# ── Qt Plugin Deployment ────────────────────────────────────────────────────
# Qt6 requires platform plugins (e.g. qwindows.dll) in a platforms/
# subdirectory next to the executable. Without them, the app fatally crashes
# at startup with "no Qt platform plugin could be initialized".
if(WIN32)
    # Use Qt6's built-in deploy support (CMake 3.21+, Qt 6.3+)
    if(QT_VERSION VERSION_GREATER_EQUAL "6.3" OR Qt6_VERSION VERSION_GREATER_EQUAL "6.3")
        qt_generate_deploy_app_script(
            TARGET VisionApp
            OUTPUT_SCRIPT deploy_script
            NO_UNSUPPORTED_PLATFORM_ERROR
            NO_TRANSLATIONS
        )
        install(SCRIPT ${deploy_script})
    endif()

    # Always add a post-build copy for local development/debugging.
    # This ensures plugins are present even without running the install step.
    set(_qt_plugin_base "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")

    foreach(_plugin_dir platforms styles imageformats)
        add_custom_command(TARGET VisionApp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:VisionApp>/${_plugin_dir}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "$<IF:$<CONFIG:Debug>,${_qt_plugin_base}/debug/Qt6/plugins/${_plugin_dir},${_qt_plugin_base}/Qt6/plugins/${_plugin_dir}>"
                "$<TARGET_FILE_DIR:VisionApp>/${_plugin_dir}"
            COMMENT "Deploying Qt ${_plugin_dir} plugins"
        )
    endforeach()
endif()

# ── IDE grouping ─────────────────────────────────────────────────────────────
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${VISIONAPP_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"     PREFIX "Source Files" FILES ${VISIONAPP_SOURCES})
source_group("Resources" FILES ${VISIONAPP_RESOURCES})

set_target_properties(VisionApp PROPERTIES
    OUTPUT_NAME "VisionStudio"
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
    MACOSX_BUNDLE_INFO_STRING "Cross-platform OCR & Image Processing Studio"
    MACOSX_BUNDLE_BUNDLE_NAME "VisionStudio"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.visionstudio.app"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2026 VisionStudio Team. All rights reserved."
)

# ── Install Rules ───────────────────────────────────────────────────────────
install(TARGETS VisionApp
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

# On Windows, install all dependent runtime DLLs (OpenCV, Qt, etc.) next to the executable
if(WIN32)
    install(FILES $<TARGET_RUNTIME_DLLS:VisionApp> DESTINATION bin)
endif()
