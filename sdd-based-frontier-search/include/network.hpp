#ifndef EQOPT_GRAPH_HPP
#define EQOPT_GRAPH_HPP

#include <unordered_set>
#include <utility>
#include <vector>

#include "../tdzdd/util/MessageHandler.hpp"
#include "../tdzdd/util/Graph.hpp"

using namespace tdzdd;

using Edge = std::pair<int, int>;

class Network {
 public:
  int n;
  int m;
  std::vector<Edge> e;  // 表示图中的所有边

  Network(int _n) : n(_n), m(0){};
  Network() : n(0), m(0){};

  int numE() const { return m; }
  int numV() const { return n; }

  bool readfromFile(const char *filename) {
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL) {
      return false;
    }
    m = n = 0;
    int u, v;
    while (fscanf(fp, "%d%d", &u, &v) != EOF) {
      if (u > v) std::swap(u, v);
      e.emplace_back(u, v);
      n = std::max(n, u);
      n = std::max(n, v);
      ++m;
    }
    fclose(fp);
    e.shrink_to_fit();  // 将e中多余的空间释放
    return true;
  };
};

void read_network(tdzdd::Graph &graph, const char *filename_graph,
                  const char *filename_terminal);

#endif  // EQOPT_GRAPH_HPP
