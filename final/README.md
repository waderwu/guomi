## GPU
- nvcc GPU_SM4.cu GPU_SM4_TEST.cu -O3 -o GPU_SM4

## Static
- g++ Static_SM4.cpp Static_SM4_TEST.cpp  -std=c++11 -O3 -o Static_SM4

## OpenMP
- g++ OpenMP_SM4.cpp OpenMP_SM4_TEST.cpp  -std=c++11 -O3 -o OpenMP_SM4  -fopenmp

## AVX
- g++ AVX_SM4.cpp AVX_SM4_TEST.cpp -fpermissive  -mavx2 -Wa,-q -std=c++11  -O3 -o AVX_SM4
