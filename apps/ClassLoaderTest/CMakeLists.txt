add_library(TestLibrary SHARED TestLibrary.cpp)

include_directories(
${PROJECT_SOURCE_DIR}/src
)
set(TARGET_NAME ClassLoaderTest)
set_property( GLOBAL APPEND PROPERTY LIBS2COMPILE  " ${CMAKE_SHARED_LIBRARY_PREFIX}TestLibrary${CMAKE_SHARED_LIBRARY_SUFFIX}")
set_property( GLOBAL APPEND PROPERTY APPS2COMPILE  " ${TARGET_NAME}")

add_executable(ClassLoaderTest TestPlugin.cpp)
target_link_libraries(ClassLoaderTest pi_base)

