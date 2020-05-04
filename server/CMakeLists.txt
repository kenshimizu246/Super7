
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

file(GLOB_RECURSE SOURCE_FILES "*.cpp"  )

message(${SOURCE_FILES})
message("CMAKE_CXX_COMPILER:" ${CMAKE_CXX_COMPILER})

add_definitions(" -g -Wall -std=c++11 ")

include_directories(
	/usr/local/include
	/usr/include
	/usr/lib64
)

add_executable(super7
src/Config.cpp
src/App.cpp
src/vl53l0x/I2Cdev.cpp
src/vl53l0x/VL53L0X.cpp
src/worker/vl53l0x_worker.cpp
src/worker/hcsr04_worker.cpp
src/worker/gy271_worker.cpp
src/pca9685.cpp
src/message.cpp
	)
target_link_libraries(super7
	pthread
	wiringPi
	uuid
        boost_system
	)



