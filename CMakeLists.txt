cmake_minimum_required(VERSION 3.7)
project(ctest)


find_package(JNI)

if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
endif()
include_directories(${JNI_INCLUDE_DIRS})
link_directories(${JNI_INCLUDE_DIRS})

add_library(jni SHARED "D:/projects/ctest/jni.h")
set_target_properties(jni PROPERTIES LINKER_LANGUAGE CXX)

add_library(jvm.lib SHARED "C:/Program Files/Java/jdk1.8.0_121_64/lib/jvm.lib")
set_target_properties(jvm.lib PROPERTIES LINKER_LANGUAGE CXX)


set(SOURCE_FILES main.cpp)
add_executable(ctest ${SOURCE_FILES})
target_link_libraries(ctest jvm.lib jni ${JNI_LIBRARIES} )


