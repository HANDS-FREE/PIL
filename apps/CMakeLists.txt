set(APPS2BUILD CameraTest ClassLoaderTest GUI_Test Svar_Test Timer_Test Tests VideoCompare Map2DFusion ObstacleAvoidance)
#  SE3_Test

foreach(APPNAME ${APPS2BUILD})
	if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${APPNAME})
		add_subdirectory(${APPNAME})
	endif()
endforeach()
