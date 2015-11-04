# CMake version requirement
cmake_minimum_required(VERSION 2.8)

project( LgMesh )

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

aux_source_directory(. SRC_SOURCE)

# subdirectory
#add_subdirectory(IsoEx)
add_subdirectory(LgMeshLib)
add_subdirectory(LgMeshTest)