add_executable(VBT Blob.cpp
Blob.h
Main.cpp
)

target_link_libraries( VBT ${OpenCV_LIBRARIES} )
     
install (TARGETS VBT DESTINATION bin)
