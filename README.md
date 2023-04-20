# MAP-UOT
## Background
Unbalanced optimal transport (UOT) has been widely used as a key algorithm in many application domains, where it often dominates the application running time. It is becoming a tool in a wider range of application domains utilizing machine learning (ML), such as computational biology, computational imaging, neuroimaging, natural language processing, domain adaptation, supervised learning, etc. We optimize UOT according to its memory-bound characteristics by interweaving the row rescaling and column rescaling that were originally performed independently to substantially reduce the memory access.

## Software dependences
* [GNU Compiler (GCC)](https://gcc.gnu.org/)
* [NVCC](https://docs.nvidia.com/cuda/cuda-compiler-driver-nvcc/index.html)
* [POSIX Threads](https://en.wikipedia.org/wiki/Pthreads)
* [CMake >= 3.9](https://cmake.org/)

## Hardware platform
12th Gen Intel Core i9-12900K|\                     |\                 |\            |
-----------------------------|----------------------|------------------|-------------|
Number of Cores              | 16                   | Frequency        | 3.1 GHz     |    
Peak Performance             | 793.6 GFLOPS (FP32)  | Memory Bandwidth | 76.8 GB/s   |    
OS kernel                    | Linux 5.15           | Compiler         | GCC 9.4.0   |

NVIDIA GeForce RTX 3090 Ti |\                |\                  |\                  |
---------------------------|-----------------|-------------------|-------------------|
GPU Boost Clock            | 1.86 GHz        |  Peak Performance | 40 TFLOPS (FP32)  |
Memory                     | 384-bit GDDR6X  |  Memory Capacity  | 24 GB             |
Memory Clock               | 1.313 GHz       |  Memory Bandwidth | 1008 GB/s         |
CUDA Cores                 | 10752           |  CUDA Version     | 11.4              |
NVIDIA GPU Driver          | 515.65          |  Compiler         | NVCC V11.4.48     |

Tianhe-1 supercomputer     |\                |\                  |\                  |
---------------------------|-----------------|-------------------|-------------------|
CPU                        | Intel Xeon Westmere    |  Cores per Node    | 12        |
Max Frequency              | 2.93 GHz               |  Memory Capacity   | 32 GB     |
Interconnect Network       | Infiniband QDR         |  Network Bandwidth | 160 Gb/s  |

## Compile and install
The program can be compiled by the following command.
```bash
$ mkdir build
$ cd build
$ cmake ../src
$ make
```

## Running
* Every executable file has a corresponding .sh file with the same name.
For example, you can measure GPU platform performance improvements with MAP-UOT_GPU.sh.
```bash
$ cd scripts
$ sh MAP-UOT_GPU.sh
```

## Result
When finishing executing, you can find running time results in .log file with the same name.
For example, MAP-UOT_GPU.log.
