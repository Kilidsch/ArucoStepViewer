cmake_minimum_required(VERSION 3.16.0)

project(aruco_viewer VERSION 3.1.2 LANGUAGES CXX)

list(APPEND COMMON_COMPILE_OPTIONS
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wextra>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-pedantic>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W3>
    $<$<CXX_COMPILER_ID:MSVC>:/EHsc>
    $<$<CXX_COMPILER_ID:MSVC>:/permissive->
    $<$<CXX_COMPILER_ID:MSVC>:/WX>
)


find_package(OpenCV REQUIRED)
message(STATUS "Using OpenCV version ${OpenCV_VERSION}")
message(STATUS "${OpenCV_LIBS}")
find_package(FLTK REQUIRED)



add_executable(aruco_viewer)
target_link_libraries(aruco_viewer PRIVATE ${OpenCV_LIBS} fltk)
target_include_directories(aruco_viewer PRIVATE ${FLTK_INCLUDE_DIRS})

# FLTK needs this
if (APPLE)
  target_link_libraries(hello "-framework cocoa")
endif (APPLE)

target_compile_options(aruco_viewer PRIVATE ${COMMON_COMPILE_OPTIONS})

target_sources(aruco_viewer PRIVATE src/main.cpp src/gui.h src/gui.cpp)
