#include "../include/network.hpp"

void read_network(tdzdd::Graph &graph, const char *filename_graph,
                  const char *filename_terminal) {
  Network G;  // 确定BDD变量顺序后的网络图
  int n, m;
  G.readfromFile(filename_graph);
  n = G.numV();
  m = G.numE();

  std::unordered_set<int> srcs;  // 存储对应的源点
  FILE *fp;
  fp = fopen(filename_terminal, "r");
  int src;
  while (fscanf(fp, "%d", &src) != EOF) {
    srcs.emplace(src);
  }
  fclose(fp);

  // 将Network G转化为TdZdd包中的Graph graph
  for (const auto &edg : G.e) {
    graph.addEdge(std::to_string(edg.first), std::to_string(edg.second));
  }
  graph.update();

  for (const auto &src : srcs) {
    graph.setColor(std::to_string(src), 1);
  }
  graph.update();
}