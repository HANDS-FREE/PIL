add_library(TestLibrary SHARED TestLibrary.cpp)

include_directories(
${PROJECT_SOURCE_DIR}/src
)
add_executable(ClassLoaderTest TestPlugin.cpp)
target_link_libraries(ClassLoaderTest pi_base)

