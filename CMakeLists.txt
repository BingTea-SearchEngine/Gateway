cmake_minimum_required(VERSION 3.31)

set(THIS Gateway)

project(${THIS} CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

add_library(Common ${LIB_DIR}/Common.cpp)
target_include_directories(Common PUBLIC ${LIB_DIR})

add_library(GatewayServer ${LIB_DIR}/Server/GatewayServer.cpp)
target_include_directories(GatewayServer PUBLIC ${LIB_DIR}/Server)
target_link_libraries(GatewayServer PUBLIC pthread Common)

add_library(GatewayClient ${LIB_DIR}/Client/GatewayClient.cpp)
target_include_directories(GatewayClient PUBLIC ${LIB_DIR}/Client)
target_link_libraries(GatewayClient PUBLIC Common)

add_executable(Server src/Server.cpp)
target_link_libraries(Server PUBLIC GatewayServer)

add_executable(Client src/Client.cpp)
target_link_libraries(Client PUBLIC GatewayClient)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
