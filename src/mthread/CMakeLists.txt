#头文件直接include就好 无需生成库文件
include_directories(${project_source_dir}/include/)

set(LIBRARY_OUTPUT_PATH ${ROOT_NAME}/lib)
aux_source_directory(. DIR_MTHREAD)
add_library(mthread SHARED ${DIR_MTHREAD})

find_package(Threads)
TARGET_LINK_LIBRARIES(mthread ${CMAKE_THREAD_LIBS_INIT})

