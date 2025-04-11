# 在CMakeLists.txt开头添加（放在cmake_minimum_required之后）
cmake_minimum_required(VERSION 3.5)
project(TestQt)  # 添加这行

# 项目配置
project(QtButtonDemo
    VERSION 1.0
    LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动处理Qt的MOC、UIC和RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt5组件
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# 添加可执行文件
add_executable(${PROJECT_NAME}
    src/main.cpp
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
)

# 安装规则（可选）
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)