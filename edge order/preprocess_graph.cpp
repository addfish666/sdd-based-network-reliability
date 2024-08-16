#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <utility>
#include <algorithm>
#include <string>
#include <unordered_set>

/*
argv[1]: input graph
argv[2]: output graph
删除数据集中存在问题的图，存在问题的图是指图中存在度数为0的孤立节点
*/
int main(int argc, char **argv){
  int m = 0; // 边数
  int n = 0; // 顶点数
  std::vector<std::pair<int, int>> edges;// 存储边，边表示为顶点对
  
  if(argc <= 2){
    exit(EXIT_FAILURE);
  }
  
  FILE *fp;
  
  // 读取输入图的顶点数和边数
  int tmp1, tmp2;
  if((fp = fopen(argv[1], "r")) == NULL){
    exit(EXIT_FAILURE);
  }
  while(fscanf(fp, "%d%d", &tmp1, &tmp2) != EOF){
    n = std::max(n, tmp1);
    n = std::max(n, tmp2);
    ++m;
    edges.emplace_back(tmp1, tmp2);
  }
  fclose(fp);
  
  std::vector<std::unordered_set<int>> ids(n+1);// ids(i-degrees)存储第i个节点与哪一些边相邻
  for(int i=0; i<m; ++i){
    ids[edges[i].first].emplace(i);
    ids[edges[i].second].emplace(i);// 边的编号0...m-1
  }

  int cnt = 0;
  std::vector<int> renum(n+1, 0);
  for(int i=1; i<=n; ++i){
    if(ids[i].size() >= 1){
      renum[i] = ++cnt;
      /*
      由于相关的节点删除之后，数量发生了变化，需要重新编号
      将度大于等于1的节点重新编号，从1开始
      */
    }
  }
  
  if((fp = fopen(argv[2], "w")) == NULL){
    exit(EXIT_FAILURE);
  }
  // tmp.graph文件中的第一行是缩点后的图的节点数和边数
  // fprintf(fp, "%d %d\n", cnt, m);// 写入缩点后的图的节点数和边数
  for(int i=0; i<m; ++i){
    // 遍历所有的边，由于删除了度为0的点，并且对顶点进行了重新编号，因此需要更新原来边的顶点标号
    int tmp1 = renum[edges[i].first];
    int tmp2 = renum[edges[i].second];
    if (tmp1 > tmp2) std::swap(tmp1, tmp2);
    fprintf(fp, "%d %d\n", tmp1, tmp2);
  }
  fclose(fp);
  
  return 0;
}