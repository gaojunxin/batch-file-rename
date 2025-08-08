cmake_minimum_required(VERSION 3.15)
project(batch-file-rename CXX)
set(CMAKE_BUILD_PARALLEL_LEVEL 4)

# 强制使用C++11标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# set(wxWidgets_USE_STATIC 1)

# 设置绝对路径
get_filename_component(WX_ROOT "${CMAKE_SOURCE_DIR}/libs/wxWidgets" ABSOLUTE)
# 自动查找lib/cmake/wxWidgets和lib64/cmake/wxWidgets
list(APPEND CMAKE_PREFIX_PATH "${WX_ROOT}/lib/cmake/wxWidgets")
list(APPEND CMAKE_PREFIX_PATH "${WX_ROOT}/lib64/cmake/wxWidgets")
list(APPEND CMAKE_PREFIX_PATH "${WX_ROOT}")

# 明确指定组件
set(wxWidgetsComponentsRequired core base)


if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

add_executable(${PROJECT_NAME} WIN32 src/main.cpp src/MainFrame.cpp app.rc)


find_package(wxWidgets REQUIRED COMPONENTS ${wxWidgetsComponentsRequired} CONFIG)
if(wxWidgets_FOUND)
    message(STATUS "wxWidgets found: ${wxWidgets_VERSION}")
    target_link_libraries(${PROJECT_NAME} PRIVATE wxWidgets::wxWidgets)
    # target_compile_definitions(${PROJECT_NAME} PRIVATE WXUSINGDLL=0)

else()
    message(FATAL_ERROR "wxWidgets not found! Check ${WX_ROOT}")
endif()

find_package(OpenGL REQUIRED )


# 子目录方式
# target_link_libraries(${PROJECT_NAME} wx::net wx::core wx::base ssh2)
# target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})





