cmake_minimum_required(VERSION 3.20)
project(MyCppProject LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB VID_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/vid/*.cpp")
file(GLOB UTI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp")
file(GLOB AUD_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/audio/*.cpp")

set(ALL_SOURCES ${SRC_FILES} ${VID_FILES} ${UTI_FILES} ${AUD_FILES} )
add_executable(main ${ALL_SOURCES})

target_include_directories(main PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/SDL3/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/vlc/include/"
)

target_link_libraries(main PRIVATE stdc++exp)
target_link_libraries(main PRIVATE ole32)
target_link_libraries(main PRIVATE mmdevapi)
target_link_libraries(main PRIVATE uuid)
target_link_libraries(main PRIVATE ksuser)

target_link_libraries(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/SDL3/lib/x64/SDL3.lib")
target_link_libraries(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/vlc/lib/vlc.lib")
 
target_link_options(main PRIVATE "-static")

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/plugins"
        "$<TARGET_FILE_DIR:main>/plugins"
    COMMENT "Copying plugins to build directory"
)
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/libvlc.dll"
        "$<TARGET_FILE_DIR:main>/libvlc.dll"
   
    COMMENT "Copying libvlc.dll to build directory"
)
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/libvlccore.dll"
        "$<TARGET_FILE_DIR:main>/libvlccore.dll"
   
    COMMENT "Copying libvlccore.dll to build directory"
)
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/include/SDL3/lib/x64/SDL3.dll"
        "$<TARGET_FILE_DIR:main>/SDL3.dll"
   
    COMMENT "Copying libvlccore.dll to build directory"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(main PRIVATE "-fdiagnostics-color=always")
endif()