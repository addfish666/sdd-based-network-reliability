#!/bin/bash

# 清理之前的文件
rm -f output/*.dot

# 编译
g++ -O2 -Wall -w main.cpp -Iinclude -Llib -lsdd -lm -o main

# 定义网络图名字列表
networks=("Switch" "VtlWavenet2008" "RedBestel" "Globenet" "Intellifiber" "Uninett2011" "VtlWavenet2011" "Uninett2010" "Tw" "Pern" "Interoute" "Ion" "DialtelecomCz" "Deltacom" "Colt" "TataNld" "UsCarrier" "GtsCe" "Cogentco" "grid4x4" "grid5x5" "grid6x6" "grid6x12" "grid6x18" "grid6x24" "grid6x30" "grid6x36" "grid6x42" "grid6x48" "grid6x54" "grid6x60" "grid7x7")

# 获取传递的参数
k_value="$1"
print_arg="$2"

# 遍历网络图名字列表
for i in `seq 1 20`
do
    >"size.${i}.txt"
    >"time.${i}.txt"
    for network in "${networks[@]}"; do
        case $k_value in
            2)
                # 1.k = 2
                ./main "./input/${network}.txt" "./input/${network}.txt.two.${i}.src" "size.${i}.txt" "time.${i}.txt" "$print_arg"
                ;;
            half)
                # 2.k = half
                ./main "./input/${network}.txt" "./input/${network}.txt.half.${i}.src" "size.${i}.txt" "time.${i}.txt" "$print_arg"
                ;;
            all)
                # 3.k = all
                ./main "./input/${network}.txt" "./input/${network}.txt.all.1.src" "size.${i}.txt" "time.${i}.txt" "$print_arg"
                ;;
            *)
                echo "Invalid parameter: $k_value"
                exit 1
                ;;
        esac
    done
done

# 删除不需要的文件
rm -f main