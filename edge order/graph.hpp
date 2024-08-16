#ifndef EQOPT_GRAPH_HPP
#define EQOPT_GRAPH_HPP


#include <unordered_map>
#include <vector>
#include <utility>
#include <unordered_set>

using Edge = std::pair<int, int>;

class Graph{
public:
    int n;
    int m;
    std::vector<Edge> e;// 表示图中的所有边
  
    Graph(int _n): n(_n), m(0) {};
    Graph(): n(0), m(0) {};
  
    int numE() const {
        return m;
    }
    int numV() const {
        return n;
    }

    bool readfromFile(const char *filename) {
        FILE *fp;
        if((fp = fopen(filename, "r")) == NULL) {
            return false;
        }
        int u, v;
        while(fscanf(fp, "%d%d", &u, &v) != EOF) {
            if(u > v) std::swap(u, v);
            n = std::max(n, v);
            e.emplace_back(u, v);
            ++m;
        }
        fclose(fp);
        e.shrink_to_fit();
        return true;
    }
};
#endif // EQOPT_GRAPH_HPP