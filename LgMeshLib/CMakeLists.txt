cmake_minimum_required( VERSION 2.8 )
project( LgMeshLib )

# EIGEN FILES
SET( EIGEN_DIR ${PROJECT_SOURCE_DIR}/../extern/ )

FILE( GLOB Project_SRCS "core/*.*" "IO/*.*" "Utility/*.*" )		
SET( Project_INCLUDE_DIR core/ IO/ Utility/ )

INCLUDE_DIRECTORIES( ${Project_INCLUDE_DIR} 
                     ${CMAKE_CURRENT_BINARY_DIR} 
                     ${EIGEN_DIR} )

ADD_LIBRARY( ${PROJECT_NAME} STATIC ${Project_SRCS} )