## 环境配置
- 测试环境： Ubuntu 16.04 cuda V9.1.85
- gpu: GeForce GTX 950M
- 编译命令：
  - nvcc test_constant.cu -o test_constant
  - nvcc test_share.cu -o test_share

- 自带了一个测试工具： nvprof ./test_constant


## sm4_constant.cu
- 将四个table定义为constat变量（只测试了加密n个blcok的速度）
- 当加密的block的数量为1<<20 的时候速度大致为:4009.020296 Mb/s
- 当加密的block的数量为1<<25 的时候速度大致为:4580.753677 Mb/s
- 当加密的blcok的数量大于1<<25的时候就会跑飞

## sm4_share.cu
- 将四个table定以为一个wrap共享变量，赋值采用直接赋值，不用涉及到从内存里面重新读出来
- 加密的block的数量为1<<20 的时候速度大致为：4044.489383
- 当加密的block的数量为1<<25 的时候速度大致为:4616.605352 Mb/s
