set(APPS2BUILD CameraTest ClassLoaderTest GUI_Test Svar_Test Timer_Test Tests)
#  SE3_Test

foreach(APPNAME ${APPS2BUILD})
add_subdirectory(${APPNAME})
endforeach()
