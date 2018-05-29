# AVX SM4 Experiment
使用Inter AVX2指令集对SM4算法的实现
参考gmssl 2.0并修改了其中的输出部分
输入一次最少为16 blocks(128\*16bits)而不是原来的1 block（理论上最小8block也可以，后8 blocks用\x00填充）
密钥仍为128bit
输出对应为16 blocks
romalgol的很多原来的算法没有用到，尤其是优化过的4个Sbox没有用到，由于我自己没有完全理解4个Sbox的原理，所以暂时没有使用，因此理论上可能还有提升空间

GCC compile:
`g++ exp_sm4fast.cpp -fpermissive -I . -mavx2 -Wa,-q -std=c++11 -lstdc++ -o exp_sm4fast`
