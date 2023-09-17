cmake_minimum_required(VERSION 3.15)
project( CVTracking )

#file(GLOB CLIENT_SOURCES "src/*.cpp" "include/*.h")
#file(GLOB SERV_SOURCES "serv_src/*.cpp")

set( CMAKE_CXX_STANDARD 17 )
find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )
#include_directories( include )

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

add_executable( CVTracking src/CVTracking.cpp src/Vector3.cpp include/Vector3.h )
add_executable( Server serv_src/EchoTestServer.cpp )

target_include_directories(CVTracking PRIVATE include)

target_link_libraries(CVTracking Boost::program_options)
target_link_libraries( CVTracking ${OpenCV_LIBS} )
