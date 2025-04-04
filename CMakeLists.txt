# CMakeList.txt : CMake project for Emu64, include source and define
# project specific logic here.
#

cmake_minimum_required (VERSION 3.12)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED True)

file (GLOB SOURCE_FILES CONFIGURE_DEPENDS "src/*.cpp")
file (GLOB HEADERS CONFIGURE_DEPENDS "include/*.hpp")
file (GLOB LINUX_SOURCE_FILES CONFIGURE_DEPENDS "src/linux/*.cpp")
file (GLOB WINDOWS_SOURCE_FILES CONFIGURE_DEPENDS "src/windows/*.cpp")

project ("Emu64" VERSION 0.0.1)
# Add source to this project's executable.
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
	add_executable (emu  ${SOURCE_FILES} ${WINDOWS_SOURCE_FILES} ${HEADERS})
	target_include_directories(emu PRIVATE "include")
elseif((CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux") OR 
		(CMAKE_HOST_SYSTEM_NAME STREQUAL "Android"))
	add_executable (emu ${SOURCE_FILES} ${LINUX_SOURCE_FILES} ${HEADERS})
	target_include_directories(emu PRIVATE "include")
else()
	message(FATAL_ERROR "Unknown platfrom ${CMAKE_HOST_SYSTEM_NAME}. Can't build")
endif()
# TODO: Add tests and install targets if needed.
