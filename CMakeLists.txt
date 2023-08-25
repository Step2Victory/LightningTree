cmake_minimum_required(VERSION 3.0.0)
project(LightningTree VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

find_package (glog 0.6.0 REQUIRED)
find_package (yaml-cpp REQUIRED)

add_executable(LightningTree src/Vector.cpp src/AbstractTree/AbstractTree.cpp src/LTBuilder.cpp src/LightningTree/LightningTree.cpp main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

target_link_libraries (LightningTree glog::glog)
target_link_libraries (LightningTree yaml-cpp)