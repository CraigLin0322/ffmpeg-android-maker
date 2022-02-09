# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("ffmpegdemo")

#set(CMAKE_VERBOSE_MAKEFILE on)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

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
        ffmpegdemo
        # Type of a library
        SHARED
        # All cpp files to compile
        native-lib.cpp)

target_link_libraries(
        # Library to link
        ffmpegdemo
        # List of libraries to link against:
        # Library for writing messages in LogCat
        ${log-lib}

        avutil       #工具库(大部分需要)
        swresample    #音频采样数据格式转换
        avcodec      #编解码(重要)
        avfilter      #滤镜特效处理
        swscale       #视频像素数据格式转换
        avdevice     #各种设备的输入输出
        avformat     #封装格式处理
        # postproc     #后加工

        # FFmpeg libraries
        ${ffmpeg_libs_names})
