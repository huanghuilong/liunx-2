
# cmake_minimum_required(VERSION 3.5)
# project(TestQt)  # 添加这行

# # 项目配置
# project(ChatClient
#     VERSION 1.0
#     LANGUAGES CXX
# )

# # 设置C++标准
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # 自动处理Qt的MOC、UIC和RCC
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
# set(CMAKE_AUTOUIC ON)

# # 定义了一个SRC_LIST变量，包含了该目录下所有的源文件
# aux_source_directory(./src SRC_LIST)

# # 查找Qt5组件
# find_package(Qt5 COMPONENTS Widgets REQUIRED Network Core)

# # 添加可执行文件
# add_executable(${PROJECT_NAME}
#     src/main.cpp
# )

# # 链接Qt库
# target_link_libraries(${PROJECT_NAME}
#     Qt5::Widgets
#     Qt5::Network
#     Qt5::Core
#     pthread
# )

# # # 安装规则（可选）
# # install(TARGETS ${PROJECT_NAME}
# #     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# # )

# # 安装规则（可选）
# install(TARGETS ${PROJECT_NAME} DESTINATION bin)


# # cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# # 包含头文件目录
# include_directories(
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

# # 创建可执行文件
# # add_executable(${PROJECT_NAME} ${SOURCES})
 
# ####################################################################
cmake_minimum_required(VERSION 3.5)
project(ChatClient
    VERSION 1.0
    LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt依赖
find_package(Qt5 COMPONENTS Widgets Network Core REQUIRED)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 递归收集所有源文件
file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)

# 递归收集所有头文件（用于IDE显示）
file(GLOB_RECURSE HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# 添加可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}  # 可选，方便IDE索引
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Qt5::Network
    Qt5::Core
    pthread
)

# 安装规则
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

# 可选：自动检测新文件变化
# configure_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.h.in
#     ${CMAKE_CURRENT_BINARY_DIR}/version.h
# )