add_subdirectory(./mthread)
add_subdirectory(./hash)
add_subdirectory(./type)
add_subdirectory(./mapreduce)

set(EXECUTABLE_OUTPUT_PATH ${ROOT_NAME}/bin)

#头文件直接include就好 无需生成库文件
# include_directories(${project_source_dir}/include/command_)
aux_source_directory(. DIR_SRC)

# 添加一个可编译的目标到工程
add_executable (${PROJECT_NAME} ${DIR_SRC})

#file(GLOB LIBS "${ROOT_NAME}/lib*.a")
#target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})


target_link_libraries(${PROJECT_NAME} hash)
target_link_libraries(${PROJECT_NAME} mapreduce)
target_link_libraries(${PROJECT_NAME} mytype)
target_link_libraries(${PROJECT_NAME} mthread)
