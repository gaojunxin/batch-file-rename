cmake_minimum_required(VERSION 3.15)
project(batch-file-rename CXX)
set(CMAKE_BUILD_PARALLEL_LEVEL 4)

# 强制使用C++11标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置绝对路径
get_filename_component(WX_ROOT "${CMAKE_SOURCE_DIR}/libs/wxWidgets" ABSOLUTE)
# 自动查找lib/cmake/wxWidgets和lib64/cmake/wxWidgets
list(APPEND CMAKE_PREFIX_PATH "${WX_ROOT}/lib/cmake/wxWidgets")
list(APPEND CMAKE_PREFIX_PATH "${WX_ROOT}/lib64/cmake/wxWidgets")
list(APPEND CMAKE_PREFIX_PATH "${WX_ROOT}")

# 明确指定组件
set(wxWidgetsComponentsRequired core base)

add_executable(${PROJECT_NAME} src/main.cpp
    src/MainFrame.cpp
    src/MainFrame.h
 )

find_package(wxWidgets REQUIRED COMPONENTS ${wxWidgetsComponentsRequired} CONFIG)
if(wxWidgets_FOUND)
    message(STATUS "wxWidgets found: ${wxWidgets_VERSION}")
    target_link_libraries(${PROJECT_NAME} PRIVATE wxWidgets::wxWidgets)
else()
    message(FATAL_ERROR "wxWidgets not found! Check ${WX_ROOT}")
endif()

find_package(OpenGL REQUIRED )


# 子目录方式
# target_link_libraries(${PROJECT_NAME} wx::net wx::core wx::base ssh2)
# target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})



if (WIN32)
    message(STATUS "current system is windows32")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS,\"5.01\"")
endif()


