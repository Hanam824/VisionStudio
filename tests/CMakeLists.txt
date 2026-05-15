# ── Tests ────────────────────────────────────────────────────────────────────
cmake_minimum_required(VERSION 3.21)

find_package(GTest REQUIRED)

# Gather test sources.
file(GLOB_RECURSE TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(VisionTests ${TEST_SOURCES} ../src/VisionCore/src/ImageProcessor.cpp)

target_link_libraries(VisionTests
    PRIVATE
        VisionCore
        GTest::gtest
        GTest::gtest_main
)

# Register with CTest.
include(GoogleTest)
gtest_discover_tests(VisionTests)
