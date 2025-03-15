cmake_minimum_required(VERSION 3.16.0)

project(aruco_viewer VERSION 3.1.2 LANGUAGES CXX)

list(APPEND COMMON_COMPILE_OPTIONS
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wextra>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-pedantic>
#    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W3>
    $<$<CXX_COMPILER_ID:MSVC>:/EHsc>
    $<$<CXX_COMPILER_ID:MSVC>:/permissive->
    $<$<CXX_COMPILER_ID:MSVC>:/WX>
    $<$<CXX_COMPILER_ID:MSVC>:/MT>
)

find_package(OpenCV REQUIRED)
find_package(FLTK CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)

message(STATUS "Using OpenCV version ${OpenCV_VERSION}")
message(STATUS "${OpenCV_LIBS}")

add_library(viewer_core STATIC src/aruco.cpp src/aruco.h src/testimages.h src/testimages.cpp src/source.h src/source.cpp)

include_directories(src)
target_compile_features(viewer_core PUBLIC cxx_std_23)
target_link_libraries(viewer_core PUBLIC ${OpenCV_LIBS} $<$<CXX_COMPILER_ID:GNU>:atomic>)
target_compile_options(viewer_core PRIVATE ${COMMON_COMPILE_OPTIONS})

add_executable(fltk_viewer
    src/fltk_main.cpp
    src/fltk_parameters.h
    src/fltk_parameters.cpp
    src/sceneview.h
    src/sceneview.cpp
    src/ratetimer.h
    src/imageviewer.h
    src/imageviewer.cpp)
target_link_libraries(fltk_viewer PRIVATE viewer_core fltk::fltk fltk::images argparse::argparse)
target_compile_options(fltk_viewer PRIVATE ${COMMON_COMPILE_OPTIONS})
target_compile_features(fltk_viewer PUBLIC cxx_std_23)
