get_filename_component(TARGET_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" TARGET_NAME pi_${TARGET_NAME})

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
	cmake_minimum_required(VERSION 2.8)
	project(${TARGET_NAME})
endif()

# Collect all source files
file(GLOB_RECURSE SOURCE_FILES_ALL RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp *.c *.cc)
#message("Found " ${SOURCE_FILES_ALL} "at folder" ${CMAKE_CURRENT_LIST_DIR})

if(SOURCE_FILES_ALL)
	include_directories(${CMAKE_CURRENT_LIST_DIR}/..)
	add_library(${TARGET_NAME} SHARED ${CMAKE_CURRENT_LIST_DIR} ${SOURCE_FILES_ALL})

	target_link_libraries(${TARGET_NAME} pthread dl)

endif()
