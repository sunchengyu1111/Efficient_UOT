all: UOT_CPU_new_pthreads UOT_GPU_new_0

UOT_CPU_new_pthreads: UOT_CPU_new_pthreads.cpp
  g++ -std=c++17 -mavx2 -O3 -o UOT_CPU_new_pthreads UOT_CPU_new_pthreads.cpp -lpthread

UOT_GPU_new_0: UOT_GPU_new_0.cu
  nvcc -arch sm_86 -O3 -o UOT_GPU_new_0 UOT_GPU_new_0.cu
