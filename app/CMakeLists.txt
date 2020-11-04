# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)
#定义一个变量并给变量
set(PATH_TO_MEDIACORE ${CMAKE_SOURCE_DIR}/src/main/cpp)
# 将符合表达式的文件列表存储到变量中
file(GLOB SRC_LIST ${PROJECT_SOURCE_DIR}/src/main/cpp/*.cpp)
file(GLOB PLAYER_SRC_LIST ${PROJECT_SOURCE_DIR}/src/main/cpp/player/*.cpp)
file(GLOB SDL_SRC_LIST ${PROJECT_SOURCE_DIR}/src/main/cpp/sdl/*.cpp)
file(GLOB EGL_SRC_LIST ${PROJECT_SOURCE_DIR}/src/main/cpp/egl/*.cpp)
file(GLOB VIDEO_RENDER_SRC_LIST ${PROJECT_SOURCE_DIR}/src/main/cpp/videorender/*.cpp)
file(GLOB AUDIO_RENDER_SRC_LIST ${PROJECT_SOURCE_DIR}/src/main/cpp/audiorender/*.cpp)
file(GLOB SOUND_TOUCH_SRC_LIST ${PROJECT_SOURCE_DIR}/src/main/cpp/soundtouch/*.cpp)
file(GLOB REMUX_SRC_LIST ${PROJECT_SOURCE_DIR}/src/main/cpp/remux/*.cpp)
# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        ${SRC_LIST}
        # Provides a relative path to your source file(s).
        ${PLAYER_SRC_LIST}
        ${EGL_SRC_LIST}
        ${SDL_SRC_LIST}
        ${VIDEO_RENDER_SRC_LIST}
        ${AUDIO_RENDER_SRC_LIST}
        ${SOUND_TOUCH_SRC_LIST}
        ${REMUX_SRC_LIST})
#avcodec
add_library(
        avcodec

        SHARED

        IMPORTED

)

SET_TARGET_PROPERTIES(
        avcodec
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/src/main/jni/${ANDROID_ABI}/libavcodec.so
)

SET_TARGET_PROPERTIES(avcodec PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI})

#TARGET_INCLUDE_DIRECTORIES(
#        avcodec
#        PRIVATE
#        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI}/include
#)
#avformat
add_library(
        avformat

        SHARED

        IMPORTED
)
SET_TARGET_PROPERTIES(
        avformat
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/src/main/jni/${ANDROID_ABI}/libavformat.so
)
#TARGET_INCLUDE_DIRECTORIES(
#        avformat
#        PRIVATE
#        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI}/include
#)
SET_TARGET_PROPERTIES(avformat PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI})
#avutil
add_library(
        avutil

        SHARED

        IMPORTED
)
SET_TARGET_PROPERTIES(
        avutil
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/src/main/jni/${ANDROID_ABI}/libavutil.so
)
#TARGET_INCLUDE_DIRECTORIES(
#        avutil
#        PRIVATE
#        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI}/include
#)

SET_TARGET_PROPERTIES(avutil PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI})
add_library(
        avfilter

        SHARED

        IMPORTED
)
SET_TARGET_PROPERTIES(
        avfilter
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/src/main/jni/${ANDROID_ABI}/libavfilter.so
)
#TARGET_INCLUDE_DIRECTORIES(
#        avfilter
#        PRIVATE
#        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI}/include
#)
SET_TARGET_PROPERTIES(avfilter PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI})
add_library(
        swresample

        SHARED

        IMPORTED
)
SET_TARGET_PROPERTIES(
        swresample
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/src/main/jni/${ANDROID_ABI}/libswresample.so
)
#TARGET_INCLUDE_DIRECTORIES(
#        swresample
#        PRIVATE
#        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI}/include
#)
SET_TARGET_PROPERTIES(swresample PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI})
add_library(
        swscale

        SHARED

        IMPORTED
)

SET_TARGET_PROPERTIES(
        swscale
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/src/main/jni/${ANDROID_ABI}/libswscale.so
)
#TARGET_INCLUDE_DIRECTORIES(
#        swscale
#        PRIVATE
#        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI}/include
#)
SET_TARGET_PROPERTIES(swscale PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI})
add_library(
        postproc

        SHARED

        IMPORTED
)

SET_TARGET_PROPERTIES(
        postproc
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/src/main/jni/${ANDROID_ABI}/libpostproc.so
)
#TARGET_INCLUDE_DIRECTORIES(
#        postproc
#        PRIVATE
#        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI}/include
#)
SET_TARGET_PROPERTIES(postproc PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/${ANDROID_ABI})
add_library(
        x264

        STATIC

        IMPORTED
)

SET_TARGET_PROPERTIES(
        x264
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/src/main/jni/${ANDROID_ABI}/libx264.a
)
#TARGET_INCLUDE_DIRECTORIES(
#        x264
#        PRIVATE
#        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/x264/${ANDROID_ABI}
#)
SET_TARGET_PROPERTIES(x264 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/x264/${ANDROID_ABI})

add_library(
        fdk-aac

        STATIC

        IMPORTED
)
SET_TARGET_PROPERTIES(
        fdk-aac
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/src/main/jni/${ANDROID_ABI}/libfdk-aac.a)
#TARGET_INCLUDE_DIRECTORIES(
#        fdk-aac
#        PRIVATE
#        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/fdkaac/${ANDROID_ABI}
#)
SET_TARGET_PROPERTIES(
        fdk-aac
        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/fdkaac/${ANDROID_ABI})
add_library(
        vpx

        STATIC

        IMPORTED
)
SET_TARGET_PROPERTIES(
        vpx
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/src/main/jni/${ANDROID_ABI}/libvpx.a)
SET_TARGET_PROPERTIES(
        fdk-aac
        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/vpx/${ANDROID_ABI})
add_library(
        sox

        STATIC

        IMPORTED
)

SET_TARGET_PROPERTIES(
        sox
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/src/main/jni/${ANDROID_ABI}/libsox.a)

add_library(
        vo-aacenc

        STATIC

        IMPORTED
)

SET_TARGET_PROPERTIES(
        vo-aacenc
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/src/main/jni/${ANDROID_ABI}/libvo-aacenc.a)
include_directories(
        ${PATH_TO_MEDIACORE}/include
        ${PATH_TO_MEDIACORE}/include/common
        ${PATH_TO_MEDIACORE}/include/player
        ${PATH_TO_MEDIACORE}/include/sdl
        ${PATH_TO_MEDIACORE}/include/egl
        ${PATH_TO_MEDIACORE}/include/videorender
        ${PATH_TO_MEDIACORE}/include/soundtouch
        ${PATH_TO_MEDIACORE}/include/remux
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/sox/include
)
# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib
        avcodec
        avfilter
        avformat
        avutil
        swresample
        swscale
        postproc
        fdk-aac
        x264
        sox
        vo-aacenc
        android
        GLESv3
        EGL
        OpenSLES
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})