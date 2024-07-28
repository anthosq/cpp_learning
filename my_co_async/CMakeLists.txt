set(ENABLE_MODULES ON)
if (ENABLE_MODULES)
    cmake_minimum_required(VERSION 3.28)
else()
    cmake_minimum_required(VERSION 3.18)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(my_co_async LANGUAGES CXX)


