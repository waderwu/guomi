# Waderwu
## GPU
- cuda环境配置
  - https://developer.nvidia.com/cuda-downloads 可参照此页面进行安装
- 编译方式
  - nvcc GPU_SM4.cu GPU_SM4_TEST.cu -O3 -o GPU_SM4
- 调用接口
  - `void gpu_sm4_encrypt(const uint8_t *plain, const uint8_t *key, uint8_t *cipher, uint n_block);`
  - `void gpu_sm4_decrypt(uint8_t *plain, const uint8_t *key, const uint8_t *cipher, uint n_block);`
  - 注意plain，cipher变量申请空间时需要使用`cudaMallocManaged`函数，不能使用malloc直接声明，具体使用请参考`GPU_SM4_TEST.cu`

## Static
- 编译方式
  - g++ Static_SM4.cpp Static_SM4_TEST.cpp  -std=c++11 -O3 -o Static_SM4
- 调用接口
  - `void static_sm4_encrypt(const uint8_t *plain, const uint8_t *key, uint8_t *cipher);`
  - `void static_sm4_decrypt(uint8_t *plain, const uint8_t *key, const uint8_t *cipher);`

## OpenMP
- 编译方式
  - g++ OpenMP_SM4.cpp OpenMP_SM4_TEST.cpp  -std=c++11 -O3 -o OpenMP_SM4  -fopenmp
- 调用接口
  - `void openmp_sm4_encrypt(const uint8_t *plain, const uint8_t *key, uint8_t *cipher, uint n_block);`
  - `void openmp_sm4_decrypt(uint8_t *plain, const uint8_t *key, const uint8_t *cipher, uint n_block);`

## AVX
- 编译方式
  - g++ AVX_SM4.cpp AVX_SM4_TEST.cpp -fpermissive  -mavx2 -Wa,-q -std=c++11  -O3 -o AVX_SM4

## benchmark
- 方法

 测试不同block加密和解密的速度，每个测试100轮，然后取平均速度
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
- 测试机操作系统

 `Linux myubuntu 4.4.0-130-generic #156-Ubuntu SMP Thu Jun 14 08:53:28 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux`

- 测试机CPU信息

 `Intel® Core™ i7-4720HQ CPU @ 2.60GHz × 8 `

- 测试机内存

 `11.7GB`

### 测试结果
- GPU
```
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
- Static
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
- OpenMP
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

- SM4_encrypt

|        | 16     | 256    | 1024   | 4096   | 65536   | 262144  | 1048576 | 33554432 |
| ------ | ------ | ------ | ------ | ------ | ------- | ------- | ------- | -------- |
| GPU    | 3.99   | 60.54  | 206.76 | 823.45 | 2987.48 | 4128.31 | 4742.01 | 4929.83  |
| Static | 105.68 | 120.26 | 118.86 | 120.15 | 121.85  | 121.42  | 121.39  |          |
| OpenMP | 35.33  | 103.01 | 113.09 | 115.25 | 115.24  | 116.58  | 112.99  |          |
| AVX    |        |        |        |        |         |         |         |          |

- SM4_decrypt

|        | 16     | 256    | 1024   | 4096   | 65536   | 262144  | 1048576 | 33554432 |
| ------ | ------ | ------ | ------ | ------ | ------- | ------- | ------- | -------- |
| GPU    | 3.97   | 47.6   | 233.76 | 592.52 | 3023.52 | 4316.03 | 4734.59 | 4870.77  |
| Static | 102.15 | 120.45 | 118.21 | 120.66 | 121.2   | 121.69  | 121.49  |          |
| OpenMP | 36.27  | 102.85 | 113.16 | 113.57 | 116.6   | 116.16  | 111.96  |          |
| AVX    |        |        |        |        |         |         |         |          |

# cpegg
## 编译方式、调用接口保持一致

## BenchMark
- 方法
  测试不同block加密和解密的速度，每个测试100轮，然后取平均速度
- 测试机操作系统
  Windows 10家庭中文版
- 测试机CPU信息
  `Intel® Core™ i7-6500U CPU @ 2.50GHz * 4`
- 测试机内存
  `3.6GB`

### 测试结果

- Static
```
SM4_encrypt>>> blocks: 16, time: 0.000000 s, speed: inf Mb/s
SM4_decrypt>>> blocks: 16, time: 0.000000 s, speed: inf Mb/s
SM4_encrypt>>> blocks: 256, time: 0.000030 s, speed: 130.208333 Mb/s
SM4_decrypt>>> blocks: 256, time: 0.000040 s, speed: 97.656250 Mb/s
SM4_encrypt>>> blocks: 1024, time: 0.000160 s, speed: 97.656250 Mb/s
SM4_decrypt>>> blocks: 1024, time: 0.000160 s, speed: 97.656250 Mb/s
SM4_encrypt>>> blocks: 4096, time: 0.000670 s, speed: 93.283582 Mb/s
SM4_decrypt>>> blocks: 4096, time: 0.000670 s, speed: 93.283582 Mb/s
SM4_encrypt>>> blocks: 65536, time: 0.010870 s, speed: 91.996320 Mb/s
SM4_decrypt>>> blocks: 65536, time: 0.010500 s, speed: 95.238095 Mb/s
SM4_encrypt>>> blocks: 262144, time: 0.042570 s, speed: 93.962885 Mb/s
SM4_decrypt>>> blocks: 262144, time: 0.044620 s, speed: 89.645899 Mb/s
SM4_encrypt>>> blocks: 1048576, time: 0.178200 s, speed: 89.786756 Mb/s
SM4_decrypt>>> blocks: 1048576, time: 0.172190 s, speed: 92.920611 Mb/s
```

- OpenMP
```
SM4_encrypt>>> blocks: 16, time: 0.000010 s, speed: 24.414062 Mb/s
SM4_decrypt>>> blocks: 16, time: 0.000000 s, speed: inf Mb/s
SM4_encrypt>>> blocks: 256, time: 0.000040 s, speed: 97.656250 Mb/s
SM4_decrypt>>> blocks: 256, time: 0.000040 s, speed: 97.656250 Mb/s
SM4_encrypt>>> blocks: 1024, time: 0.000170 s, speed: 91.911765 Mb/s
SM4_decrypt>>> blocks: 1024, time: 0.000170 s, speed: 91.911765 Mb/s
SM4_encrypt>>> blocks: 4096, time: 0.000710 s, speed: 88.028169 Mb/s
SM4_decrypt>>> blocks: 4096, time: 0.000680 s, speed: 91.911765 Mb/s
SM4_encrypt>>> blocks: 65536, time: 0.010540 s, speed: 94.876660 Mb/s
SM4_decrypt>>> blocks: 65536, time: 0.010690 s, speed: 93.545370 Mb/s
SM4_encrypt>>> blocks: 262144, time: 0.042700 s, speed: 93.676815 Mb/s
SM4_decrypt>>> blocks: 262144, time: 0.041610 s, speed: 96.130738 Mb/s
SM4_encrypt>>> blocks: 1048576, time: 0.168450 s, speed: 94.983675 Mb/s
SM4_decrypt>>> blocks: 1048576, time: 0.167120 s, speed: 95.739588 Mb/s
```
- AVX
```
SM4_encrypt>>> blocks: 16, time: 0.000000 s, speed: inf MB/s
SM4_encrypt>>> blocks: 16, time: 0.000010 s, speed: 24.414062 MB/s
SM4_encrypt>>> blocks: 256, time: 0.000020 s, speed: 195.312500 MB/s
SM4_encrypt>>> blocks: 256, time: 0.000020 s, speed: 195.312500 MB/s
SM4_encrypt>>> blocks: 1024, time: 0.000070 s, speed: 223.214286 MB/s
SM4_encrypt>>> blocks: 1024, time: 0.000100 s, speed: 156.250000 MB/s
SM4_encrypt>>> blocks: 4096, time: 0.000340 s, speed: 183.823529 MB/s
SM4_encrypt>>> blocks: 4096, time: 0.000300 s, speed: 208.333333 MB/s
SM4_encrypt>>> blocks: 65536, time: 0.004730 s, speed: 211.416490 MB/s
SM4_encrypt>>> blocks: 65536, time: 0.004590 s, speed: 217.864924 MB/s
SM4_encrypt>>> blocks: 262144, time: 0.018490 s, speed: 216.333153 MB/s
SM4_encrypt>>> blocks: 262144, time: 0.018230 s, speed: 219.418541 MB/s
SM4_encrypt>>> blocks: 1048576, time: 0.073260 s, speed: 218.400218 MB/s
SM4_encrypt>>> blocks: 1048576, time: 0.074470 s, speed: 214.851618 MB/s
```
