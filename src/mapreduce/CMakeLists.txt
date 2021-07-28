#头文件直接include就好 无需生成库文件
include_directories(${project_source_dir}/include/)

aux_source_directory(. DIR_MAPREDUCE)
set(LIBRARY_OUTPUT_PATH ${ROOT_NAME}/lib)
add_library(mapreduce SHARED ${DIR_MAPREDUCE})

target_link_libraries(mapreduce hash)
target_link_libraries(mapreduce mthread)


