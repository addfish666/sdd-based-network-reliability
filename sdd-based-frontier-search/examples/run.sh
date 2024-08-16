#!/bin/bash 
make clean
make
./test-7
cd output/


# 定义起始数字和结束数字  
end=$1
  
# 使用for循环执行dot命令  
for ((i=1; i<=$end; i++))  
do  
    image="sdd${i}.dot"  
    output="sdd${i}.png"  
    dot -T png -o "$output" "$image"  
done

dot -T png -o vtree.png vtree.dot
cd ..
rm -f src/*.o test-* circuit circuit-gc output/*.dot output/*.sdd