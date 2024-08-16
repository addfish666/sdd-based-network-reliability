#!/bin/bash

# 检查是否提供了网络图名字参数
if [ $# -ne 1 ]; then
  echo "Usage: $0 network_name"
  exit 1
fi

# 获取传递的网络图名字参数
network="$1"

# 清理之前的文件
rm -f output/*.dot

# 编译
g++ -O2 -Wall -w main.cpp -Iinclude -Llib -lsdd -lm -o main
> size.txt
> time.txt

# 执行命令
./main "./input/${network}.txt" "./input/${network}.txt.all.1.src" "size.txt" "time.txt" 3

# 删除不需要的文件
rm -f main size.txt time.txt