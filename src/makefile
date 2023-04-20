all: MAP-UOT_CPU MAP-UOT_MPI MAP-UOT_GPU

MAP-UOT_GPU: MAP-UOT_GPU.cu
	nvcc -O3 -o MAP-UOT_GPU MAP-UOT_GPU.cu MAP-UOT_Testprint_1.cpp MAP-UOT_Testtypical_1.cpp MAP-UOT_TestGPUMap.cu

MAP-UOT_MPI: MAP-UOT_MPI.cpp
	mpic++ -mavx2 -O3 -Wall -o MAP-UOT_MPI MAP-UOT_MPI.cpp MAP-UOT_Testprint_0.cpp MAP-UOT_TestMPItypical.cpp MAP-UOT_TestMPIMap.cpp

MAP-UOT_CPU: MAP-UOT_CPU.cpp
	g++ -mavx2 -O3 -Wall -o MAP-UOT_CPU MAP-UOT_CPU.cpp MAP-UOT_Testprint_0.cpp MAP-UOT_Testtypical_0.cpp MAP-UOT_TestMap.cpp -lpthread
