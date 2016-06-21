cmake_minimum_required(VERSION 2.8.6)
project(PIL)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=native ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall   -O3 -march=native")

# Check C++11 or C++0x support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   add_definitions(-DCOMPILEDWITHC11)
   message(STATUS "Using flag -std=c++11.")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
   add_definitions(-DCOMPILEDWITHC0X)
   message(STATUS "Using flag -std=c++0x.")
else()
   message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/scripts)

#####################################################################
# Find dependencies

#**Qt** : sudo apt-get install build-essential g++ libqt4-core libqt4-dev libqt4-gui qt4-doc qt4-designer 
#**QGLViewer** : sudo apt-get install libqglviewer-dev libqglviewer2

find_package(OpenCV 2.4.9 REQUIRED)
find_package(Eigen3 3.1.0 REQUIRED)
find_package(QGLViewer    REQUIRED)

set(QT_LIBRARIES QtGui QtCore QtXml GL GLU glut GLEW)
#**Qt** : sudo apt-get install build-essential g++ libqt4-core libqt4-dev libqt4-gui qt4-doc qt4-designer 
#**QGLViewer** : sudo apt-get install libqglviewer-dev libqglviewer2
set(PI_BASE_INCLUDES ${CMAKE_SOURCE_DIR}/src)
set(PI_BASE_LIBRARIES pi_base)
set(PI_GUI_LIBRARIES pi_gui)
#####################################################################
# Results collection
macro(PIL_CHECK_DEPENDENCY LIBNAME)
	if(NOT ${LIBNAME}_INCLUDES)
		if(${LIBNAME}_INCLUDE_DIR)
			set(${LIBNAME}_INCLUDES ${${LIBNAME}_INCLUDE_DIR})
		endif(${LIBNAME}_INCLUDE_DIR)
	endif(NOT ${LIBNAME}_INCLUDES)

	if(NOT ${LIBNAME}_LIBRARIES)
		if(${LIBNAME}_LIBRARY)
			set(${LIBNAME}_LIBRARIES ${${LIBNAME}_LIBRARY})
		elseif(${LIBNAME}_LIBS)
			set(${LIBNAME}_LIBRARIES ${${LIBNAME}_LIBS})
		endif()
	endif(NOT ${LIBNAME}_LIBRARIES)
endmacro()

macro(PIL_ECHO_LIBINFO LIBNAME)
	PIL_CHECK_DEPENDENCY(${LIBNAME})
	message(--${LIBNAME}:)
	message("     " ${${LIBNAME}_INCLUDES})
	message("     " ${${LIBNAME}_LIBRARIES})
endmacro()

macro(PIL_ECHO_LIBSINFO LIBNAMES)	
		message("LIBNAME: " ${LIBNAME})
		PIL_ECHO_LIBINFO(${LIBNAME})
	endforeach()
endmacro()


message("----------------------------------------------------------")
PIL_ECHO_LIBINFO(OPENCV)
PIL_ECHO_LIBINFO(QT)
PIL_ECHO_LIBINFO(QGLVIEWER)

message("----------------------------------------------------------")
#####################################################################
# Now do build things
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/libs)

# Build libs
add_subdirectory(src)

# Build apps
add_subdirectory(apps)

