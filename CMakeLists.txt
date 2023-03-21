cmake_minimum_required(VERSION 3.24)

# 项目名称，引入两个变量 demo_BINARY_DIR 和 demo_SOURCE_DIR，同时，cmake 自动定义了两个等价的变量 PROJECT_BINARY_DIR 和 PROJECT_SOURCE_DIR
project(pc-demo)

# add .cpp
aux_source_directory(./third-party/imgui CPP_LIST_IMGUI)
add_executable(demo
        demo.cpp
        ${CPP_LIST_IMGUI}
        )

# include openGL
find_package(OpenGL REQUIRED)
set(DEPENDENCIES
        realsense2
        glfw
        ${OPENGL_LIBRARY})
target_link_libraries(demo ${DEPENDENCIES} )

# include opencv
set(OpenCV_DIR /usr/local/share/OpenCV)
find_package(OpenCV 3.4.0 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(demo ${OpenCV_LIBS})

# include directories
include_directories(
        ./
        ./common
        ./third-party/
        ./third-party/imgui
        )


