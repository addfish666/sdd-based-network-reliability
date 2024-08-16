CXX = g++
CXXFLAGS = -c -Wall
TARGET = main 
OBJ = edge_order.o

$(TARGET): $(OBJ) graph.hpp
	$(CXX) -o $(TARGET) $(OBJ)
	
edge_order.o: edge_order.cpp graph.hpp
	$(CXX) $(CXXFLAGS) edge_order.cpp

.PHONY: clean # 防止有一个叫clean的文件存在而不会执行删除命令
clean:
	rm -f *.o $(TARGET)