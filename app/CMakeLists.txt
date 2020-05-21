# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(PATH_TO_MEDIACORE ${CMAKE_SOURCE_DIR}/src/main/cpp)

file(GLOB SRC_LIST ${PROJECT_SOURCE_DIR}/src/main/cpp/*.cpp)
file(GLOB PLAYER_SRC_LIST ${PROJECT_SOURCE_DIR}/src/main/cpp/player/*.cpp)
file(GLOB SDL_SRC_LIST ${PROJECT_SOURCE_DIR}/src/main/cpp/sdl/*.cpp)
file(GLOB EGL_SRC_LIST ${PROJECT_SOURCE_DIR}/src/main/cpp/egl/*.cpp)
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
        ${SDL_SRC_LIST})
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
add_library(
        fdk-aac

        STATIC

        IMPORTED
)

SET_TARGET_PROPERTIES(
        fdk-aac
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/src/main/jni/${ANDROID_ABI}/libfdk-aac.a)
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
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/ffmpeg/include
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/x264/include
        ${PROJECT_SOURCE_DIR}/src/main/cpp/thirdlib/fdkaac/include
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