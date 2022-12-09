cmake_minimum_required(VERSION 3.0.0)
project(LightningTree VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

add_executable(LightningTree LightningTree.cpp LTBuilder.cpp test.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
