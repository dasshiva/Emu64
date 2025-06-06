﻿# CMakeList.txt : CMake project for Emu64, include source and define
# project specific logic here.
#

cmake_minimum_required (VERSION 3.12)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED True)

file (MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/gen-headers)
file (GLOB GENERATED_HEADERS CONFIGURE_DEPENDS gen-headers/tables.cpp)
file (GLOB SOURCE_FILES CONFIGURE_DEPENDS "src/*.cpp")
file (GLOB HEADERS CONFIGURE_DEPENDS "include/*.hpp")
file (GLOB LINUX_SOURCE_FILES CONFIGURE_DEPENDS "src/linux/*.cpp")
file (GLOB WINDOWS_SOURCE_FILES CONFIGURE_DEPENDS "src/windows/*.cpp")

project ("Emu64" VERSION 0.0.1)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
	add_executable(tablegen "utils/TableGen.cpp" "utils/cfg/cfg_parse.c")
	add_custom_command(
		OUTPUT ${CMAKE_SOURCE_DIR}/gen-headers/tables.cpp
		COMMAND tablegen ${CMAKE_SOURCE_DIR}/gen-headers/tables.cpp
		DEPENDS tablegen
		COMMENT "Generating tables" 
		VERBATIM 
	)

	add_custom_target(AUTOGEN_HEADERS ALL DEPENDS ${CMAKE_SOURCE_DIR}/gen-headers/tables.cpp)
	add_executable (emu  ${SOURCE_FILES} ${WINDOWS_SOURCE_FILES} ${HEADERS}  ${CMAKE_SOURCE_DIR}/gen-headers/tables.cpp)
	add_dependencies(emu AUTOGEN_HEADERS)
	add_dependencies(emu tablegen)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
	target_include_directories(emu PRIVATE "include")
elseif((CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux") OR 
		(CMAKE_HOST_SYSTEM_NAME STREQUAL "Android"))
	add_compile_options(-fvisibility=default)

	add_executable(tablegen "utils/TableGen.cpp" "utils/cfg/cfg_parse.c")
    add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/gen-headers/tables.c ${CMAKE_SOURCE_DIR}/gen-headers/tables.h
        COMMAND tablegen ${CMAKE_SOURCE_DIR}/utils/tables.cfg ${CMAKE_SOURCE_DIR}/gen-headers/tables
        DEPENDS tablegen ${CMAKE_SOURCE_DIR}/utils/tables.cfg
        COMMENT "Generating tables"
        VERBATIM
    )

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    add_custom_target(AUTOGEN_HEADERS ALL DEPENDS 
        ${CMAKE_SOURCE_DIR}/gen-headers/tables.c 
        ${CMAKE_SOURCE_DIR}/gen-headers/tables.h)

	add_executable (emu ${CMAKE_SOURCE_DIR}/gen-headers/tables.c 
        ${SOURCE_FILES} ${LINUX_SOURCE_FILES} ${HEADERS}
        ${CMAKE_SOURCE_DIR}/gen-headers/tables.h)

	target_include_directories(emu PRIVATE "include")
    target_include_directories(emu PRIVATE "gen-headers")
	add_dependencies(emu AUTOGEN_HEADERS)
    add_dependencies(emu tablegen)

else()
	message(FATAL_ERROR "Unknown platfrom ${CMAKE_HOST_SYSTEM_NAME}. Can't build")
endif()
# TODO: Add tests and install targets if needed.
