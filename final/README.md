## GPU
- 需要安装cuda
  - https://developer.nvidia.com/cuda-downloads 可参照此页面进行安装
- nvcc GPU_SM4.cu GPU_SM4_TEST.cu -O3 -o GPU_SM4

### benchmark
- 方法
测试不同block加密和解密的速度，每个次测试的时候100轮，然后取平均数

- 测试GPU信息
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
- 测试操作系统
`Linux myubuntu 4.4.0-130-generic #156-Ubuntu SMP Thu Jun 14 08:53:28 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux`

- 测试CPU信息
`Intel® Core™ i7-4720HQ CPU @ 2.60GHz × 8 `

- 测试机内存
`11.7GB`

- 测试结果
```
0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e, 0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46,
0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e, 0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46,
0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e, 0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46,
0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e, 0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46,

0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,

SM4_encrypt>>> blocks: 16, time: 0.000061 s, speed: 3.990530
SM4_decrypt>>> blocks: 16, time: 0.000061 s, speed: 3.973643
SM4_encrypt>>> blocks: 256, time: 0.000065 s, speed: 60.543242
SM4_decrypt>>> blocks: 256, time: 0.000082 s, speed: 47.602364
SM4_encrypt>>> blocks: 1024, time: 0.000076 s, speed: 206.761943
SM4_decrypt>>> blocks: 1024, time: 0.000067 s, speed: 233.767205
SM4_encrypt>>> blocks: 4096, time: 0.000076 s, speed: 823.451910
SM4_decrypt>>> blocks: 4096, time: 0.000105 s, speed: 592.529389
SM4_encrypt>>> blocks: 65536, time: 0.000335 s, speed: 2987.482449
SM4_decrypt>>> blocks: 65536, time: 0.000331 s, speed: 3023.523009
SM4_encrypt>>> blocks: 262144, time: 0.000969 s, speed: 4128.307807
SM4_decrypt>>> blocks: 262144, time: 0.000927 s, speed: 4316.065475
SM4_encrypt>>> blocks: 1048576, time: 0.003374 s, speed: 4742.019330
SM4_decrypt>>> blocks: 1048576, time: 0.003379 s, speed: 4734.596287
SM4_encrypt>>> blocks: 33554432, time: 0.103857 s, speed: 4929.836487
SM4_decrypt>>> blocks: 33554432, time: 0.105117 s, speed: 4870.776029
```

## Static
- g++ Static_SM4.cpp Static_SM4_TEST.cpp  -std=c++11 -O3 -o Static_SM4

- benchmark
```
SM4_encrypt>>> blocks: 16, time: 0.000002 s, speed: 105.688582 Mb/s
SM4_decrypt>>> blocks: 16, time: 0.000002 s, speed: 102.150889 Mb/s
SM4_encrypt>>> blocks: 256, time: 0.000032 s, speed: 120.266318 Mb/s
SM4_decrypt>>> blocks: 256, time: 0.000032 s, speed: 120.451742 Mb/s
SM4_encrypt>>> blocks: 1024, time: 0.000131 s, speed: 118.866489 Mb/s
SM4_decrypt>>> blocks: 1024, time: 0.000132 s, speed: 118.218960 Mb/s
SM4_encrypt>>> blocks: 4096, time: 0.000520 s, speed: 120.155337 Mb/s
SM4_decrypt>>> blocks: 4096, time: 0.000518 s, speed: 120.668018 Mb/s
SM4_encrypt>>> blocks: 65536, time: 0.008207 s, speed: 121.848837 Mb/s
SM4_decrypt>>> blocks: 65536, time: 0.008251 s, speed: 121.202131 Mb/s
SM4_encrypt>>> blocks: 262144, time: 0.032941 s, speed: 121.429148 Mb/s
SM4_decrypt>>> blocks: 262144, time: 0.032868 s, speed: 121.697399 Mb/s
SM4_encrypt>>> blocks: 1048576, time: 0.131809 s, speed: 121.387793 Mb/s
SM4_decrypt>>> blocks: 1048576, time: 0.131697 s, speed: 121.491017 Mb/s
```
1<<25 次方跑的时间太长了，我注释掉了

## OpenMP
- g++ OpenMP_SM4.cpp OpenMP_SM4_TEST.cpp  -std=c++11 -O3 -o OpenMP_SM4  -fopenmp
```
SM4_encrypt>>> blocks: 16, time: 0.000007 s, speed: 35.331494 Mb/s
SM4_decrypt>>> blocks: 16, time: 0.000007 s, speed: 36.276467 Mb/s
SM4_encrypt>>> blocks: 256, time: 0.000038 s, speed: 103.012922 Mb/s
SM4_decrypt>>> blocks: 256, time: 0.000038 s, speed: 102.850184 Mb/s
SM4_encrypt>>> blocks: 1024, time: 0.000138 s, speed: 113.085330 Mb/s
SM4_decrypt>>> blocks: 1024, time: 0.000138 s, speed: 113.167234 Mb/s
SM4_encrypt>>> blocks: 4096, time: 0.000542 s, speed: 115.245611 Mb/s
SM4_decrypt>>> blocks: 4096, time: 0.000550 s, speed: 113.572350 Mb/s
SM4_encrypt>>> blocks: 65536, time: 0.008577 s, speed: 116.587756 Mb/s
SM4_decrypt>>> blocks: 65536, time: 0.008576 s, speed: 116.600263 Mb/s
SM4_encrypt>>> blocks: 262144, time: 0.034441 s, speed: 116.141827 Mb/s
SM4_decrypt>>> blocks: 262144, time: 0.034435 s, speed: 116.161827 Mb/s
SM4_encrypt>>> blocks: 1048576, time: 0.141603 s, speed: 112.992300 Mb/s
SM4_decrypt>>> blocks: 1048576, time: 0.142901 s, speed: 111.965337 Mb/s
```

## AVX
- g++ AVX_SM4.cpp AVX_SM4_TEST.cpp -fpermissive  -mavx2 -Wa,-q -std=c++11  -O3 -o AVX_SM4
