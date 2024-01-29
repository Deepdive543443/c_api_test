cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 11)
project(capi VERSION 0.1.0 LANGUAGES C CXX)


set(ncnn_DIR ../../../../cpp_libs/ncnn/build/install/lib/cmake/ncnn)
add_subdirectory(src/detector)

add_executable(capi src/main.c)
target_include_directories(capi PRIVATE src)
target_link_libraries(capi detkit)

add_executable(vec src/vec_testing.c)
target_include_directories(vec PRIVATE src)
target_link_libraries(vec detkit)


find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
