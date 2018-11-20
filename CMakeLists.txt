cmake_minimum_required(VERSION 3.12)
set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
set(CUDA_COMMON "/usr/local/cuda/samples/common/inc")

project(fir_cuda C CXX CUDA)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(CUDA 10.0 REQUIRED)
set(CMAKE_CXX_STANDARD 14)
#if(MSVC)
#    # Force to always compile with W4
#    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
#        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#    else()
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
#    endif()
#elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
#    # Update if necessary
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
#endif()
#

# set this according to your GPU architecture
# (see https://docs.nvidia.com/cuda/cuda-compiler-driver-nvcc/index.html#virtual-architecture-feature-list)
set(GPU_ARCH arch=compute_52,code=sm_52)
set(
        CUDA_NVCC_FLAGS
        ${CUDA_NVCC_FLAGS};
        -O3 -gencode ${GPU_ARCH}
)


add_executable(fir_cpu audiofir0.c audiofir.h audiofir.c)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${CUDA_COMMON})

add_executable(fir_cuda1 audiofir1.cu audiofir.h audiofir.c)
target_link_libraries(fir_cuda1 ${CUDA_LIBRARY} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

add_executable(fir_cuda2 audiofir2.cu audiofir.h audiofir.c)
target_link_libraries(fir_cuda2 ${CUDA_LIBRARY} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

add_executable(fir_cuda3 audiofir3.cu audiofir.h audiofir.c)
target_link_libraries(fir_cuda3 ${CUDA_LIBRARY} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})