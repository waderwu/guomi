# Waderwu

- 编译方式

 `gcc test_sm3.c sm3.c -O3 -o test_sm3`

 - 测试机操作系统
 
`Linux myubuntu 4.4.0-130-generic #156-Ubuntu SMP Thu Jun 14 08:53:28 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux`

 - 测试机CPU信息
 
`Intel® Core™ i7-4720HQ CPU @ 2.60GHz × 8`

 - 测试机内存
 
`11.7GB`

## benchmark
  - Static
  ```
  size: 16 time: 0.000001 s speed: 29.919194 MB/s
  size: 64 time: 0.000001 s speed: 82.479941 MB/s
  size: 256 time: 0.000002 s speed: 133.410178 MB/s
  size: 1024 time: 0.000006 s speed: 175.325404 MB/s
  size: 8192 time: 0.000033 s speed: 234.891762 MB/s
  size: 16384 time: 0.000066 s speed: 237.462006 MB/s
  size: 1048576 time: 0.004103 s speed: 243.715788 MB/s
  ```
  - AVX
  ```
Speed Test
size: 16 time: 0.000001 s speed: 61.035156 MB/s
size: 64 time: 0.000002 s speed: 122.070313 MB/s
size: 256 time: 0.000005 s speed: 195.312500 MB/s
size: 1024 time: 0.000018 s speed: 217.013889 MB/s
size: 8192 time: 0.000109 s speed: 286.697248 MB/s
size: 16384 time: 0.000212 s speed: 294.811321 MB/s
size: 1048576 time: 0.013627 s speed: 293.534894 MB/s
```
  - GmSSL
  ```
  type      16 bytes     64 bytes    256 bytes   1024 bytes   8192 bytes  16384 bytes
  sm3       42080.73k    92331.47k   153508.10k   189030.40k   197946.03k   201752.58k
  ```
# cpegg
- 编译方式

使用Windows Visual Studio x86 Release编译方式编译

 - 测试机操作系统
 
`Windows 10家庭中文版`

 - 测试机CPU信息
 
`Intel® Core™ i7-6500U CPU @ 2.50GHz * 4`

 - 测试机内存
 
`3.6GB`
## benchmark
- 编译方式

 `g++ sm3-hash-4way.c AVX_test.cpp -fpermissive -mavx2 -Wa,-q -std=c++11 -O3 -o AVX_SM3`
 
- AVX
```
size: 16 time: 0.000001 s speed: 61.035156 MB/s
size: 64 time: 0.000003 s speed: 81.380208 MB/s
size: 256 time: 0.000005 s speed: 195.312500 MB/s
size: 1024 time: 0.000019 s speed: 205.592105 MB/s
size: 8192 time: 0.000144 s speed: 217.013889 MB/s
size: 16384 time: 0.000247 s speed: 253.036437 MB/s
size: 1048576 time: 0.015283 s speed: 261.728718 MB/s
```

