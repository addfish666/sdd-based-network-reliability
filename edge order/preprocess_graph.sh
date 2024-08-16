#!/bin/bash

# 1st argument: 输入文件夹路径
# 2nd argument: 输出文件夹路径

# 输入和输出文件夹示例
# input_folder="./data"
# output_dir="./output_graph_test"
rm -f *.o main
g++ preprocess_graph.cpp -o main

# 以输入参数的方式传入输入和输出文件夹
input_folder=$1
output_dir=$2

# C++程序的可执行文件路径
program_path="./main"

# Use find command to locate all .prep files in the input_folder
input_files=($(find "$input_folder" -name "*.txt"))

for input_file in "${input_files[@]}"; do
    # 获取当前网络图文件名（不包含路径和扩展名）
    filename=$(basename -- "$input_file")

    # 定义输出文件名
    output_file="${output_dir}/${filename}"
    
    # 运行C++程序，将输入和输出文件路径作为参数
    ${program_path} ${input_file} ${output_file}
done

rm -f *.o main