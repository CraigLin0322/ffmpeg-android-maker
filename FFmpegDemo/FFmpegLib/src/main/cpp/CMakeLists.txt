# Refer to this https://proandroiddev.com/a-story-about-ffmpeg-in-android-part-ii-integration-55fb217251f0
cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("ffmpeglib")


set(ffmpeg_dir ${CMAKE_SOURCE_DIR}/../cmakeLibs)

#message(${ffmpeg_dir})

set(ffmpeg_libs ${ffmpeg_dir}/${ANDROID_ABI})
include_directories(include/${ANDROID_ABI})


set(
        # List variable name
        ffmpeg_libs_names
        # Values in the list
        avutil avformat avcodec swscale swresample avdevice avfilter)

foreach (ffmpeg_lib_name ${ffmpeg_libs_names})
    add_library(
            ${ffmpeg_lib_name}
            SHARED
            IMPORTED)
    set_target_properties(
            ${ffmpeg_lib_name}
            PROPERTIES
            IMPORTED_LOCATION
            ${ffmpeg_libs}/lib${ffmpeg_lib_name}.so)
endforeach ()

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

add_library(
        # Name for a library to build
        ffmpeglib
        # Type of a library
        SHARED
        # All cpp files to compile
        ffmpeglib.cpp
        media_producer.cpp
        media_consumer.cpp
        video_player_listener.cpp
        thread_helper.cpp)

target_link_libraries(
        # Library to link
        ffmpeglib
        # List of libraries to link against:
        # Library for writing messages in LogCat
        ${log-lib}
        android
        OpenSLES
        # FFmpeg libraries
        ${ffmpeg_libs_names})
