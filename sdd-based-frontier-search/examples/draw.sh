make
./test-9
cd output/
dot -T png -o vtree.png vtree.dot
cd ..