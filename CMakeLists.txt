cmake_minimum_required(VERSION 3.14)
project(IDW_serial)

set(CMAKE_CXX_STANDARD 14)
#target_include_directories(gdal 'usr/include/gdal/')
# find system installed GDAL package with predefined CMake variable for finding GDAL
find_package(GDAL REQUIRED)

# Specify location of GDAL header files
include_directories(include gdal)
add_executable(IDW_serial
    main.cpp
        common.h
        Raster.cpp
        Raster.h
        Metodos.cpp
        Metodos.h

    )
# Specify GDAL libraries to link your cpp executable target against
target_link_libraries(IDW_serial gdal)