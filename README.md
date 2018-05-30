# guomi

sm4fast:`gcc sm4fast.cpp -I ./ -std=c++11 -lstdc++ -o sm4fast`
openmp: `gcc sm4fast_openmp.cpp -I ./ -std=c++11 -lstdc++ -O3 -o openmp  -fopenmp`
cuda：`nvcc test_share.cu -o share`,`nvcc test_constant.cu -o constant`


大概的测试数据
- 静态内存加速(O3优化) 
  - 118MB/S
- CUDA
  - 4037 MB/s
- AVX
  - 120.041326 MB/s
 
方案|16\*1|16\*4|16\*16|16\*64|16\*512|16\*1024|16\*2^25Byte
-|-|-|-|-|-|-|-
CUDA|0.24|1.60|6.24|25.6|128.1|306.3|4032.2
MemStatic|3.05|61.0|81.3|101.4|103.4|108.5|119.6|
AVX|-|-|61|33|63|51|119.6|

单位MB/s

测试机内核版本
Linux myubuntu 4.4.0-127-generic #153-Ubuntu SMP Sat May 19 10:58:46 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux
