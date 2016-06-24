set(LIB_MODULES base network hardware gui cv)

foreach(LIB_NAME ${LIB_MODULES})
add_subdirectory(${LIB_NAME})
endforeach()
