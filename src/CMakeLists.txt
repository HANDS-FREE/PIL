add_subdirectory(base)

if(WITH_NETWORK)
add_subdirectory(network)
endif()

add_subdirectory(hardware)
add_subdirectory(gui)
add_subdirectory(cv)

