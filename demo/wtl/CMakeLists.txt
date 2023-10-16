cmake_minimum_required(VERSION 3.14)
project(WTLMocapApi)

set(CMAKE_CXX_STANDARD 17)

add_executable(demo WIN32
    mocap_console.h
    mocap_console.cpp
    stdafx.h
    stdafx.cpp
    mocap_console.rc
    resource.h
    mocap_panel.h
    mocap_panel.cpp
    mocap_api_service.h
    mocap_api_service.cpp
    mocap_application.h
    mocap_application.cpp
    debug.h
    debug.cpp
    w_a_conv.h
    w_a_conv.cpp
    settings_panel.hpp
)

target_include_directories(demo PRIVATE
    ${MOCAPAPI_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
    $ENV{DEVLIBS}/boost_1_81_0
    third/spdlog/include
    third/WTL/Include
    ../../include
)
target_compile_definitions(demo PRIVATE BOOST_DLL_USE_STD_FS UNICODE _UNICODE)
target_link_libraries(demo PRIVATE Version)

if(MSVC)
    get_filename_component(PCH_NAME stdafx.h NAME_WE)

    if(CMAKE_CONFIGURATION_TYPES)
        set(PCH_DIR "${CMAKE_CURRENT_BINARY_DIR}/PCH/${CMAKE_CFG_INTDIR}")
    else(CMAKE_CONFIGURATION_TYPES)
        set(PCH_DIR "${CMAKE_CURRENT_BINARY_DIR}/PCH")
    endif(CMAKE_CONFIGURATION_TYPES)

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/PCH)

    set_target_properties(demo PROPERTIES COMPILE_FLAGS
        "/Yustdafx.h /FIstdafx.h /Fp${PCH_DIR}/${PCH_NAME}.pch")

    set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS
        "/Ycstdafx.h")
    set_directory_properties(PROPERTIES
        ADDITIONAL_MAKE_CLEAN_FILES ${PCH_DIR}/${PCH_NAME}.pch)
endif(MSVC)