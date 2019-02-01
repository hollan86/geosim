cmake_minimum_required(VERSION 3.5)
project(GeoSim)
set (CMAKE_CXX_STANDARD 14)


##LINKING SHARED LIBRARIES
#set( PROJECT_LINK_LIBS libGL.so.1)

#glfw library
set (GLFW_LINK_LIBS libglfw.3.2.dylib)

set( GDAL_LINK_LIBS libgdal.20.dylib )

#set ( GLEW_LINK_LIBS /home/hollan/dev/gdaltuts/opengdal/GLEW/lib/libGLEW.so )
#set ( GLEW_INCLUDE_DIR GLEW/include/ )


link_directories(
        /usr/local/Cellar/glfw/3.2.1/lib/
        /usr/local/Cellar/gdal/2.3.2_1/lib/
)

#ADDING INCLUDE DIRECTORIES
include_directories(
        include
        /usr/local/Cellar/gdal/2.3.2_1/include/
)

file(GLOB SOURCES "src/*")

add_executable(geosim ${SOURCES})

target_link_libraries(geosim ${GDAL_LINK_LIBS} ${GLFW_LINK_LIBS} ${CMAKE_DL_LIBS})
