cmake_minimum_required(VERSION 3.5)
project(GeoSim)
set (CMAKE_CXX_STANDARD 14)


#LINKING SHARED LIBRARIES
set( PROJECT_LINK_LIBS libGL.so.1)

#glfw library
set( GLFW_LINK_LIBS libglfw.so )

set( GDAL_LINK_LIBS libgdal.so )

set ( GLEW_LINK_LIBS /home/hollan/dev/gdaltuts/opengdal/GLEW/lib/libGLEW.so )
set ( GLEW_INCLUDE_DIR GLEW/include/ )


link_directories(/snap/gnome-3-26-1604/74/usr/lib/x86_64-linux-gnu/mesa/ /usr/local/lib/)

#ADDING INCLUDE DIRECTORIES
include_directories(/usr/local/include/ include /usr/include/GL ${GLEW_INCLUDE_DIR})

file(GLOB SOURCES "src/*")

add_executable(geosim ${SOURCES})

target_link_libraries(geosim ${PROJECT_LINK_LIBS} ${GLEW_LINK_LIBS} ${GDAL_LINK_LIBS} ${GLFW_LINK_LIBS}  -lglut -lGLU -lGL ${CMAKE_DL_LIBS})
