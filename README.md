# guomi

- sm4fast:`gcc sm4fast.cpp -I ./ -std=c++11 -lstdc++ -o sm4fast`
- openmp: `gcc sm4fast_openmp.cpp -I ./ -std=c++11 -lstdc++ -O3 -o openmp  -fopenmp`
- cuda：`nvcc test_share.cu -o share`,`nvcc test_constant.cu -o constant`


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

CPU|SPEED
-|-
cpuA|156.861207
cpuB|120.107496

CPU|16\*1|16\*4|16\*16|16\*64|16\*512|16\*1024|16\*2^25Byte
-|-|-|-|-|-|-|-
cpuC|3.051758|61.035156|81.380208|51.398026|120.192308|123.031496|130.410029

- cpuA：Intel(R) Core(TM) i9-7900X CPU @ 3.30GHz
- cpuB：Intel(R) Core(TM) i7-4720HQ CPU @ 2.60GHz
- cpuC: Intel(R) Xeon(R) CPU E5-2643 v3 @ 3.40GHz
- 测试时文件大小为33554432 Byte

单位MB/s
- cuda size 536870912 Byte

thread|share|constant
-|-|-
32|2118.898338|4081.925521
64|3401.699521|4729.136840
128|4430.560483|4693.759683
256|4486.544747|4570.938828
512|4423.058649|4377.228154
1024|4034.005405|4104.274217

GPU相关参数
```
Device Name : GeForce GTX 950M.
totalGlobalMem : 2100232192.
sharedMemPerBlock : 49152.
regsPerBlock : 65536.
warpSize : 32.
memPitch : 2147483647.
maxThreadsPerBlock : 1024.
maxThreadsDim[0 - 2] : 1024 1024 64.
maxGridSize[0 - 2] : 2147483647 65535 65535.
totalConstMem : 65536.
major.minor : 5.0.
clockRate : 1124000.
textureAlignment : 512.
deviceOverlap : 1.
multiProcessorCount : 5.
```

测试机内核版本
Linux myubuntu 4.4.0-127-generic #153-Ubuntu SMP Sat May 19 10:58:46 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux

cuda 版本
```
nvcc: NVIDIA (R) Cuda compiler driver
Copyright (c) 2005-2017 NVIDIA Corporation
Built on Fri_Nov__3_21:07:56_CDT_2017
Cuda compilation tools, release 9.1, V9.1.85
```

gmssl部分速度测试
- sms4
```
type             16 bytes     64 bytes    256 bytes   1024 bytes   8192 bytes  16384 bytes
sms4 cbc         93230.45k    93896.32k    93871.87k    93982.04k    94183.42k    94240.77k
```
-sm3
```
type             16 bytes     64 bytes    256 bytes   1024 bytes   8192 bytes  16384 bytes
sm3              42156.40k    92510.49k   153072.98k   186386.77k   200029.53k   201900.03k
```
crypto++ sm3速度
```
SM3
size: 16 
time: 0.000003 s
speed: 5.086263 
size: 64 
time: 0.000001 s
speed: 61.035156 
size: 256 
time: 0.000002 s
speed: 122.070312 
size: 1024 
time: 0.000009 s
speed: 108.506944 
size: 8192 
time: 0.000049 s
speed: 159.438776 
size: 16384 
time: 0.000089 s
speed: 175.561798 
size: 1048576 
time: 0.004606 s
speed: 217.108120 
```
sm3fast.c
```
size: 16 
time: 0.000004 s
speed: 3.814697 
size: 64 
time: 0.000002 s
speed: 30.517578 
size: 256 
time: 0.000002 s
speed: 122.070312 
size: 1024 
time: 0.000006 s
speed: 162.760417 
size: 8192 
time: 0.000040 s
speed: 195.312500 
size: 16384 
time: 0.000081 s
speed: 192.901235 
size: 1048576 
time: 0.004456 s
speed: 224.416517 
```
sm3 手动展开的
```
size: 16 
time: 0.000006 s
speed: 2.543132 
size: 64 
time: 0.000002 s
speed: 30.517578 
size: 256 
time: 0.000004 s
speed: 61.035156 
size: 1024 
time: 0.000007 s
speed: 139.508929 
size: 8192 
time: 0.000044 s
speed: 177.556818 
size: 16384 
time: 0.000087 s
speed: 179.597701 
size: 1048576 
time: 0.004130 s
speed: 242.130751 
```
