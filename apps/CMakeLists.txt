set(APPS2BUILD CameraTest ClassLoaderTest GUI_Test Svar_Test Timer_Test Tests VideoCompare Map2DFusion ObstacleAvoidance)
#  SE3_Test

file(GLOB APPS2BUILD "*")
message("APPS2BUILD: " ${APPS2BUILD})

foreach(APPNAME ${APPS2BUILD})
	if(IS_DIRECTORY ${APPNAME})
		add_subdirectory(${APPNAME})
	endif()
endforeach()
