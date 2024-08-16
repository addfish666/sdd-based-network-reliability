#!/bin/bash

# 检查是否提供了 arg_m 参数
if [ $# -ne 1 ]; then
  echo "Usage: $0 <vtree_arg_m>"
  exit 1
fi

# 将输入的 arg_m 参数赋值
arg_m=$1

# 数组定义每个测试用例的参数
# test_cases=("Eunetworks 16" "Spiralight 16" "Nextgen 19" "Oxford 26" "Funet 30" "Darkstrand 31" "Sunet 32" "Shentel 35" "Bren 38" "NetworkUsa 39" "IowaStatewideFiberMap 41" "PionierL1 41" "LambdaNet 46" "Intranetwork 51" "RoedunetFibre 52" "Ntelos 58" "Palmetto 64" "Syringa 74" "UsSignal 78" "Missouri 83" "Switch 92" "VtlWavenet2008 92" "RedBestel 93" "Intellifiber 95" "VtlWavenet2011 96" "Interoute 146" "Ion 146" "DialtelecomCz 151" "Deltacom 161" "TataNld 186" "UsCarrier 189")
test_cases=("grid4x4 16" "grid5x5 40" "grid6x6 60" "grid6x12 126" "grid6x18 192" "grid6x24 258" "grid6x30 324" "grid6x36 390" "grid6x42 456" "grid6x48 522" "grid6x54 588" "grid6x60 654" "grid7x7 84" "Switch 92" "VtlWavenet2008 92" "RedBestel 93" "Globenet 95" "Intellifiber 95" "Uninett2011 96" "VtlWavenet2011 96" "Uninett2010 101" "Tw 115" "Pern 129" "Interoute 146" "Ion 146" "DialtelecomCz 151" "Deltacom 161" "Colt 177" "TataNld 186" "UsCarrier 189" "GtsCe 193" "Cogentco 243")
# 编译 C++ 程序
g++ -w construct_vtree.cpp -o construct_vtree

# 循环遍历每个测试用例并运行程序
for params in "${test_cases[@]}"; do
    # 以空格为分隔符拆分参数
    IFS=' ' read -ra arr <<< "$params"
    
    # 编译并运行程序
    ./construct_vtree "${arr[@]}" $arg_m
done

# 清理生成的可执行文件
rm construct_vtree
