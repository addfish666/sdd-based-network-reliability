#!/bin/bash

# 检查是否提供了两个参数
if [ $# -ne 2 ]; then
  echo "Usage: $0 {2|half|all} print_arg"
  exit 1
fi

# 获取传递的参数
k_value="$1"
print_arg="$2"

# 检查 print_arg 是否为 1 或 2
if [[ "$print_arg" != "1" && "$print_arg" != "2" ]]; then
  echo "Invalid print_arg: $print_arg. It must be either 1 or 2."
  exit 1
fi

# 根据传入的参数调用 run_batch_20.sh 并传递 print_arg
case $k_value in
    2)
        ./run_batch_20.sh 2 "$print_arg"
        ;;
    half)
        ./run_batch_20.sh half "$print_arg"
        ;;
    all)
        ./run_batch_20.sh all "$print_arg"
        ;;
    *)
        echo "Invalid parameter: $k_value"
        exit 1
        ;;
esac

# 编译和运行平均值计算程序
g++ -o main average.cpp
./main

rm -f main
for i in {1..20}; do
    rm -f "time.${i}.txt"
    rm -f "size.${i}.txt"
done
