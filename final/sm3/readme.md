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
- AVX
```
size: 16 time: 0.000001 s speed: 61.035156 MB/s
size: 64 time: 0.000002 s speed: 122.070313 MB/s
size: 256 time: 0.000004 s speed: 244.140625 MB/s
size: 1024 time: 0.000015 s speed: 260.416667 MB/s
size: 8192 time: 0.000119 s speed: 262.605042 MB/s
size: 16384 time: 0.000239 s speed: 261.506276 MB/s
```