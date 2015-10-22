cmake_minimum_required( VERSION 2.8 )
project( LgMesh )

# EIGEN FILES
FIND_PATH( EIGEN_DIR "eigen" )

FILE( GLOB Project_SRCS "src/*.*")
						
SET( Project_INCLUDE_DIR src/)

INCLUDE_DIRECTORIES( ${Project_INCLUDE_DIR} 
                     ${CMAKE_CURRENT_BINARY_DIR} 
                     ${EIGEN_DIR} )

ADD_EXECUTABLE( ${PROJECT_NAME} 
				${Project_SRCS} )
				
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} )