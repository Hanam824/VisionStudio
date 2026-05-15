# ── VisionCore Shared Library ────────────────────────────────────────────────
cmake_minimum_required(VERSION 3.21)

# Gather sources
file(GLOB_RECURSE VISIONCORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE VISIONCORE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# Create shared library
add_library(VisionCore SHARED
    ${VISIONCORE_SOURCES}
    ${VISIONCORE_HEADERS}
)

# Define the export macro so headers emit __declspec(dllexport) when building.
target_compile_definitions(VisionCore PRIVATE VISIONCORE_EXPORTS)

# Public include directory (consumers get the /include folder).
target_include_directories(VisionCore
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link OpenCV
target_link_libraries(VisionCore
    PUBLIC
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
)

# Link ncnn if available
if(ncnn_FOUND)
    target_link_libraries(VisionCore PRIVATE ncnn)
    target_compile_definitions(VisionCore PRIVATE VISIONCORE_HAS_NCNN)
endif()

# Link ONNX Runtime if available
if(onnxruntime_FOUND)
    target_link_libraries(VisionCore PRIVATE onnxruntime::onnxruntime)
    target_compile_definitions(VisionCore PRIVATE VISIONCORE_HAS_ONNXRT)
endif()

# ── IDE grouping (Visual Studio filters) ────────────────────────────────────
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${VISIONCORE_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"     PREFIX "Source Files" FILES ${VISIONCORE_SOURCES})

# Set output name
set_target_properties(VisionCore PROPERTIES
    OUTPUT_NAME        "VisionCore"
    DEBUG_POSTFIX      "d"
    VERSION            ${PROJECT_VERSION}
    SOVERSION          0
)

# ── Install Rules ───────────────────────────────────────────────────────────
install(TARGETS VisionCore
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(DIRECTORY include/ DESTINATION include)
