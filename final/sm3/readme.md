 - 编译方式

 `gcc test_sm3.c sm3.c -O3 -o test_sm3`


## benchmark

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
