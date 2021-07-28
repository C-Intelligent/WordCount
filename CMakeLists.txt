# cmake最低版本号要求
cmake_minimum_required (VERSION 3.5)

# 设置编译模式
# SET(CMAKE_BUILD_TYPE "Debug")

# remove last end of "/"
set(ROOT_NAME ${CMAKE_CURRENT_SOURCE_DIR})
string(REGEX REPLACE "/$" "" CURRENT_FOLDER_ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR})
# get current relative dir name
string(REGEX REPLACE ".*/(.*)" "\\1" PROJECT_FOLDER ${CURRENT_FOLDER_ABSOLUTE})
# get_filename_component(PROJECT_FOLDER ${CURRENT_FOLDER_ABSOLUTE} NAME)

message(${PROJECT_FOLDER})

# 设置PROJECT_NAME变量
set(PROJECT_NAME ${PROJECT_FOLDER})
# 设置工程名
project (${PROJECT_NAME})

add_subdirectory(./src)

