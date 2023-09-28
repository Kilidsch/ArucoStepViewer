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
)


find_package(OpenCV REQUIRED)
message(STATUS "Using OpenCV version ${OpenCV_VERSION}")
message(STATUS "${OpenCV_LIBS}")
find_package(Qt6 REQUIRED COMPONENTS Gui Qml Quick QuickControls2)
#qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


qt_add_executable(aruco_viewer)
qt_add_qml_module(aruco_viewer
    URI stepviewer
    VERSION 1.0.0
    QML_FILES
        "src/main.qml"
    RESOURCE_PREFIX /
    SOURCES src/imagemodel.h src/imagemodel.cpp
    SOURCES src/imageitem.h src/imageitem.cpp
    QML_FILES src/DoubleSpinBox.qml
    QML_FILES src/ArucoDetectorParams.qml
    SOURCES src/arucoparamscontroller.h src/arucoparamscontroller.cpp
    QML_FILES src/DoubleParameter.qml
    QML_FILES src/IntegerParameter.qml
    )

include_directories(src)
target_compile_features(aruco_viewer PUBLIC cxx_std_20)
target_link_libraries(aruco_viewer PRIVATE ${OpenCV_LIBS} Qt::Qml Qt::Quick Qt::Gui Qt::QuickControls2)
target_compile_options(aruco_viewer PRIVATE ${COMMON_COMPILE_OPTIONS})
target_sources(aruco_viewer PRIVATE src/main.cpp src/aruco.cpp src/aruco.h src/testimages.h src/testimages.cpp)
