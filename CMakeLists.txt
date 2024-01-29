cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 11)
project(capi VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(src/detector)


file(GLOB SOURCES src/*)
add_executable(capi ${SOURCES})
target_include_directories(capi PRIVATE src)
target_link_libraries(capi detkit)
# set(ncnn_DIR ../../cpp_libs/ncnn-20240102-full-source/build/install/lib/cmake/ncnn)
# find_package(ncnn REQUIRED)
# target_link_libraries(capi ncnn)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
