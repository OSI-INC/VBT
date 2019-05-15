cmake_minimum_required(VERSION 2.6)

project(VBT)

add_compile_options(-std=c++11)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIR})

link_directories(${OpenCV_LIBRARY_DIR})

include_directories(${VBT_DIR})

add_executable(
	VBT 
	Blob.cpp
	Blob.h
	Main.cpp
)

target_link_libraries( 
	VBT 
	${OpenCV_LIBRARIES} 
)
     
install(
	TARGETS 
	VBT 
	DESTINATION 
	bin
)

