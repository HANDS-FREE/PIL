get_filename_component(TARGET_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" TARGET_NAME pi_${TARGET_NAME})

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
	cmake_minimum_required(VERSION 2.8.6)
	project(${TARGET_NAME})
endif()

# Collect all source files
file(GLOB_RECURSE SOURCE_FILES_ALL RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp *.c *.cc)
#message("Found " ${SOURCE_FILES_ALL} "at folder" ${CMAKE_CURRENT_LIST_DIR})

if(SOURCE_FILES_ALL)
	set(CMAKE_AUTOMOC TRUE)
	include_directories(${QT_INCLUDES} ${QGLVIEWER_INCLUDE_DIR} ${CMAKE_CURRENT_LIST_DIR}/..)
	add_library(${TARGET_NAME} SHARED ${CMAKE_CURRENT_LIST_DIR} ${SOURCE_FILES_ALL})

	target_link_libraries(${TARGET_NAME} pi_base ${QGLViewer_LIBRARIES} ${QT_LIBRARIES})

endif()
