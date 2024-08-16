#!/bin/bash
rm -f *.o main
g++ generate_terminal.cpp -o main

# 设置要运行C++程序的次数
num_runs=1

# C++程序的可执行文件路径
program_path="./main"

# 1st argument: 输入文件夹路径
# 2nd argument: 输出文件夹路径
# 3nd argument: 产生终端节点的数量的类型，分别为：two、half、all

# 以输入参数的方式传入输入和输出文件夹
input_folder=$1
output_dir=$2

# Use find command to locate all .prep files in the input_folder
input_files=($(find "$input_folder" -name "*.txt"))

# 获取./data文件夹下所有的网络图文件名，并进行遍
for input_file in "${input_files[@]}"; do
    # 获取当前网络图文件名（不包含路径和扩展名）
    filename=$(basename -- "$input_file")
    # filename="${filename%.*}"

    # 循环运行程序多次
    for ((i=1; i<=$num_runs; i++))
    do
        # 定义输出文件名
        output_file="${output_dir}/${filename}.$3.${i}.src"

        # 运行C++程序，将输入和输出文件路径作为参数
        ${program_path} ${input_file} ${output_file} $3

    done
done

rm -f *.o main