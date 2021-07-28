#头文件直接include就好 无需生成库文件
include_directories(${project_source_dir}/include/)

set(LIBRARY_OUTPUT_PATH ${ROOT_NAME}/lib)
aux_source_directory(. DIR_HASH)
add_library(hash SHARED ${DIR_HASH})

