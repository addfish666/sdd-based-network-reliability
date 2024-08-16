#!/bin/bash 
make clean
g++ -O2 -Wall src/test-8.cpp -I../include -L../lib -lsdd -lm -o test-8

./test-8 ./src/real-network/graphkk.txt ./src/real-network-terminal/graphk_t.txt
# ./test-8 ./src/real-network/graph.txt ./src/real-network-terminal/graph_t.txt

cd output/

dot -T png -o vtree.png vtree.dot

dot -T png -o sdd.png sdd.dot
cd ..
# rm -f src/*.o test-* circuit circuit-gc output/*.dot output/*.sdd
rm -f src/*.o test-* circuit circuit-gc