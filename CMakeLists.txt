cmake_minimum_required(VERSION 3.14)
project(IDW_OpenMP)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fopenmp")
#target_include_directories(gdal 'usr/include/gdal/')
# find system installed GDAL package with predefined CMake variable for finding GDAL
find_package(GDAL REQUIRED)

# Specify location of GDAL header files
include_directories(include gdal)
add_executable(IDW_OpenMP
    main.cpp
        common.h
        Raster.cpp
        Raster.h
        Metodos.cpp
        Metodos.h

    )
# Specify GDAL libraries to link your cpp executable target against
target_link_libraries(IDW_OpenMP gdal)