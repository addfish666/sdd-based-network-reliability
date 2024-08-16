#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
using namespace std;

// 定义边的数据结构
struct Edge {
  int source;
  int target;
};

// 读取txt文件并存储边的信息
std::vector<Edge> readEdgesFromFile(const std::string& file_path) {
  std::vector<Edge> edges;
  std::ifstream file(file_path);

  int source, target;
  while (file >> source >> target) {
    edges.push_back({source, target});
  }

  return edges;
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cerr << "Usage: " << argv[0] << " <input_file_path>" << endl;
    return 1;
  }
  // 读取txt文件，将边的信息存储在edges中
  string file_path = argv[1];
  std::vector<Edge> edges = readEdgesFromFile(file_path);

  // 计算图中顶点的数量
  int max_vertex = 0;
  for (const auto& edge : edges) {
    max_vertex = std::max(max_vertex, std::max(edge.source, edge.target));
  }

  // 输出节点数和边数
  // int num_nodes = max_vertex;
  // int num_edges = edges.size();

  // cout << "Number of nodes in the network graph: " << num_nodes << endl;
  // cout << "Number of edges in the network graph: " << num_edges << endl;

  int num_terminal_vertices = 0;

  if (strcmp(argv[3], "two") == 0) {
    num_terminal_vertices = 2;
  } else if (strcmp(argv[3], "half") == 0) {
    num_terminal_vertices = max_vertex / 2;
  } else if (strcmp(argv[3], "all") == 0) {
    num_terminal_vertices = max_vertex;
  } else {
    printf("Invalid argument for argv[3]\n");
    return 1;  // 返回错误代码
  }

  // 创建包含所有可能顶点的向量
  std::vector<int> all_vertices(max_vertex);
  for (int i = 0; i < max_vertex; ++i) {
    all_vertices[i] = i + 1;
  }

  // 使用std::random_device获取真正的随机数种子
  std::random_device rd;
  // 使用随机数种子初始化随机数引擎
  std::default_random_engine rng(rd());
  // 将所有顶点打乱
  std::shuffle(all_vertices.begin(), all_vertices.end(), rng);

  // 选择前num_terminal_vertices数量的点作为终端顶点
  std::vector<int> terminal_vertices(
      all_vertices.begin(), all_vertices.begin() + num_terminal_vertices);

  // 将选定的终端顶点写入另一个.src文件中
  std::string output_file = argv[2];
  std::ofstream output(output_file);

  for (int i = 0; i < num_terminal_vertices; ++i) {
    output << terminal_vertices[i] << "\n";
  }
  return 0;
}