cmake_minimum_required(VERSION 3.15)
project( CVTracking )
set( CMAKE_CXX_STANDARD 17 )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
add_executable( CVTracking CVTracking.cpp )
add_executable( Server EchoTestServer.cpp )

target_link_libraries(CVTracking Boost::program_options)
target_link_libraries( CVTracking ${OpenCV_LIBS} )
