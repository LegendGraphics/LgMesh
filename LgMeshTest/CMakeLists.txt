cmake_minimum_required( VERSION 2.8 )
project( LgMeshTest )

# EIGEN FILES
SET( EIGEN_DIR "../LgMeshLib/extern" )

# LGMESH FILES
SET( LgMeshLib_DIR "../LgMeshLib" )
SET( LgMeshLib_debug 	  ${PROJECT_SOURCE_DIR}/../lib/Debug/LgMeshLib.lib )
SET( LgMeshLib_release 	${PROJECT_SOURCE_DIR}/../lib/Release/LgMeshLib.lib )

# OPENGL FILES
FIND_PACKAGE( OpenGL REQUIRED ) # Currently enough
#FIND_PACKAGE( GLEW REQUIRED )

# GLFW and GLEW FILES
SET( GLFW_INCLUDE_DIR "extern/glfw-win64/include" )
SET( GLFW_LIB         ${PROJECT_SOURCE_DIR}/extern/glfw-win64/lib-vc2012/glfw3.lib )

# GLEW FILES
#SET( GLEW_INCLUDE_DIR "extern/glew-1.11.0/include" )
#SET( GLEW_LIB         ${PROJECT_SOURCE_DIR}/extern/glew-1.11.0/lib/Release/x64/glew32.lib )

FILE( GLOB Project_SRCS "Viewer/*.*" "Basics/*.*" )		
SET( Project_INCLUDE_DIR "basics/" )

INCLUDE_DIRECTORIES( ${Project_INCLUDE_DIR} 
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${LgMeshLib_DIR}
                     ${EIGEN_DIR} 
                     ${GLFW_INCLUDE_DIR} )

ADD_EXECUTABLE( ${PROJECT_NAME} 
                ${Project_SRCS} )
				
TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
           		   debug ${LgMeshLib_debug}
					   optimized ${LgMeshLib_release} 
                       ${GLFW_LIB} 
                       ${OPENGL_LIBRARIES} )