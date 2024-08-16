#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <utility>
#include <vector>

#include "graph.hpp"

using namespace std;

struct State {
  vector<int> vertex_order;  // 当前的顶点顺序
  double score;              // 评估分数，表示当前顶点顺序的质量

  bool operator<(const State& other) const {
    return score < other.score;  // 较小的评估分数优先级较高
  }

  bool operator>(const State& other) const {
    return score >
           other.score;  // 用于priority_queue中的比较，较小的评估分数优先级较高
  }
};

// 使用线性时间的启发式搜索
int LightSearch_log(const vector<int>& vertex_order,
                    const vector<vector<int>>& adjacency_list) {
  // 先找顶点V
  int v = 0;
  int sum = adjacency_list.size() - 1;
  cout << "当前顶点顺序为:";
  for (const auto& cur_vertex : vertex_order) cout << cur_vertex << " ";
  cout << "寻找v开始" << endl;
  int index = 1;
  for (const auto& cur_vertex : vertex_order) {
    int num = 0;
    // 计算 num = N(v) ∩ (V \ V')
    cout << "候选点集:[";
    for (const auto& neighbor : adjacency_list[cur_vertex]) {
      if (find(vertex_order.begin(), vertex_order.end(), neighbor) ==
          vertex_order.end()) {
        num++;
        cout << neighbor << " ";
      }
    }
    cout << "] ";
    cout << "v" << index << ":" << cur_vertex << " 得分:" << num << endl;
    index++;
    if (num != 0 && sum > num) {
      sum = num;
      v = cur_vertex;
    }
  }
  cout << "最终的v:" << v << " 得分为" << sum << endl;
  cout << "寻找v结束" << endl;
  // 根据找到的v寻找下一个顶点u
  cout << "可能的u:[";
  for (const auto& u : adjacency_list[v]) {
    if (find(vertex_order.begin(), vertex_order.end(), u) == vertex_order.end())
      cout << u << " ";
  }
  cout << "]" << endl;
  cout << "寻找u开始" << endl;
  index = 1;
  int next_vertex = 0;
  sum = adjacency_list.size() - 1;
  for (const auto& u : adjacency_list[v]) {
    if (find(vertex_order.begin(), vertex_order.end(), u) != vertex_order.end())
      continue;
    int num = 0;
    // int sum = adjacency_list.size() - 1;
    cout << "候选点集:[";
    for (const auto& neighbor : adjacency_list[u]) {
      if (find(vertex_order.begin(), vertex_order.end(), neighbor) ==
          vertex_order.end()) {
        num++;
        cout << neighbor << " ";
      }
    }
    cout << "]" << endl;
    cout << "u" << index << ":" << u << "得分:" << num << endl;
    index++;
    if (sum > num) {
      next_vertex = u;
      sum = num;
    }
  }
  cout << "最终的u:" << next_vertex << endl;
  cout << "寻找u结束" << endl;
  return next_vertex;
}

// 使用线性时间的启发式搜索
int LightSearch(const vector<int>& vertex_order,
                const vector<vector<int>>& adjacency_list) {
  // 先找顶点V
  int v = 0;
  int sum = adjacency_list.size() - 1;
  for (const auto& cur_vertex : vertex_order) {
    int num = 0;
    for (const auto& neighbor : adjacency_list[cur_vertex]) {
      if (find(vertex_order.begin(), vertex_order.end(), neighbor) ==
          vertex_order.end()) {
        num++;
      }
    }
    if (num != 0 && sum > num) {
      sum = num;
      v = cur_vertex;
    }
  }
  // 根据找到的v寻找下一个顶点
  int next_vertex = 0;
  sum = adjacency_list.size() - 1;
  for (const auto& u : adjacency_list[v]) {
    if (find(vertex_order.begin(), vertex_order.end(), u) != vertex_order.end())
      continue;
    int num = 0;
    for (const auto& neighbor : adjacency_list[u]) {
      if (find(vertex_order.begin(), vertex_order.end(), neighbor) ==
          vertex_order.end()) {
        num++;
      }
    }
    if (sum > num) {
      next_vertex = u;
      sum = num;
    }
  }
  return next_vertex;
}

// 根据LightSearch()来产生一个好的顶点顺序
vector<int> GetGoodVertexOrder(int start_vertex,
                               const vector<vector<int>>& adjacency_list) {
  vector<int> good_vertex_order;
  good_vertex_order.push_back(start_vertex);
  while (good_vertex_order.size() < adjacency_list.size() - 1) {
    int next_verter = LightSearch(good_vertex_order, adjacency_list);
    good_vertex_order.push_back(next_verter);
  }
  return good_vertex_order;
}

// 根据LightSearch()来产生一个好的顶点顺序
vector<int> GetGoodVertexOrder_log(int start_vertex,
                                   const vector<vector<int>>& adjacency_list) {
  vector<int> good_vertex_order;
  good_vertex_order.push_back(start_vertex);
  cout << "寻找最优顶点顺序迭代开始" << endl;
  int cnt = 1;
  while (good_vertex_order.size() < adjacency_list.size() - 1) {
    cout << "迭代开始" << cnt << "---------------------" << endl;
    int next_verter = LightSearch_log(good_vertex_order, adjacency_list);
    good_vertex_order.push_back(next_verter);
    cout << "当前轮次的最优下一个顶点:" << next_verter << endl;
    cout << "迭代结束" << cnt << "---------------------" << endl;
    cout << endl;
    cnt++;
  }
  return good_vertex_order;
}

// 顶点编号从1开始，最后一个参数表示处理到的当前顶点顺序中的第i个点
vector<int> GetBoundaryVertices(const vector<int>& vertex_order,
                                const vector<Edge>& edges, int i) {
  if (i == 1) return vector<int>{};
  // 从vertex_order索引0开始复制i个元素到current_vertices中，下面的区间是左闭右开的区间
  unordered_set<int> current_vertices(vertex_order.begin(),
                                      vertex_order.begin() + i);
  unordered_set<int> boundary_vertices;

  // 遍历图中的边集
  for (const auto& edge : edges) {
    int u = edge.first;
    int v = edge.second;

    // 检查边的两个顶点是否其中一个在当前顶点集合中，一个不在当前顶点集合中
    if (current_vertices.find(u) != current_vertices.end() &&
        current_vertices.find(v) == current_vertices.end()) {
      // 将在当前顶点集合中的顶点u都添加到边界顶点集合中
      boundary_vertices.insert(u);
    }
    if (current_vertices.find(u) == current_vertices.end() &&
        current_vertices.find(v) != current_vertices.end()) {
      // 将在当前顶点集合中的顶点v都添加到边界顶点集合中
      boundary_vertices.insert(v);
    }
  }

  // 将边界顶点集合转换为向量并返回
  vector<int> result(boundary_vertices.begin(), boundary_vertices.end());
  return result;
}

// 将vector<Edge> edges转化为邻接列表
vector<vector<int>> ConvertToAdjacencyList(const vector<Edge>& edges,
                                           int max_vertex) {
  // 初始化邻接表，顶点编号从1开始
  vector<vector<int>> adjacency_list(max_vertex + 1);

  // 将边集中的边添加到邻接表中
  for (const auto& edge : edges) {
    int u = edge.first;
    int v = edge.second;

    // 添加边的两个顶点到对方的邻居列表中
    adjacency_list[u].push_back(v);
    adjacency_list[v].push_back(u);
  }

  return adjacency_list;
}

double Evaluate(const vector<int>& vertex_order,
                const vector<vector<int>>& adjacency_list,
                const vector<Edge>& edges) {
  double score = 0.0;

  // 遍历每一步的顶点顺序
  for (int i = 1; i <= vertex_order.size(); i++) {
    int frontier_size = 0;
    vector<int> boundary_vertices = GetBoundaryVertices(vertex_order, edges, i);
    frontier_size = boundary_vertices.size();

    // 将当前步骤的前沿集合大小的平方添加到评估分数中
    score += frontier_size * frontier_size;
  }

  return score;
}

// 根据GetGoodVertexOrder()来计算好的起始顶点
vector<int> CalculateAppropriateStartVertices(
    int max_vertex, const vector<vector<int>>& adjacency_list,
    const std::vector<Edge>& edges) {
  vector<int> res;
  vector<State> good_vertex_states;
  for (int start_vertex = 1; start_vertex <= max_vertex; start_vertex++) {
    vector<int> good_vertex_order =
        GetGoodVertexOrder(start_vertex, adjacency_list);
    double score = Evaluate(good_vertex_order, adjacency_list, edges);
    good_vertex_states.push_back({good_vertex_order, score});
  }
  // 越小的score越在前面
  sort(good_vertex_states.begin(), good_vertex_states.end());
  for (auto& good_vertex_state : good_vertex_states) {
    if (good_vertex_state.vertex_order.size() > 0)
      res.push_back(good_vertex_state.vertex_order[0]);
  }
  return res;
}

vector<State> GenerateSuccessors(const State& state,
                                 const vector<vector<int>>& adjacency_list,
                                 int beam_width, const vector<Edge>& edges) {
  vector<State> successors;
  unordered_set<int> valid_vertex;

  // 遍历每个已有的顶点顺序
  for (const auto& order : state.vertex_order) {
    // 获取当前顶点顺序中的当前顶点
    int cur_vertex = order;

    // 遍历邻接列表中当前顶点的邻居
    for (const int& neighbor : adjacency_list[cur_vertex]) {
      // 检查新顶点是否已经存在于当前顶点顺序中
      if (find(state.vertex_order.begin(), state.vertex_order.end(),
               neighbor) == state.vertex_order.end()) {
        // 创建新的顶点顺序，需要保证这个顶点不会产生相同的后继顶点顺序
        if (!valid_vertex.count(neighbor)) {
          // 将这个合法顶点加入unordered_set<int> valid_vertex中
          valid_vertex.insert(neighbor);
          vector<int> new_order = state.vertex_order;
          new_order.push_back(neighbor);

          // 计算新顶点顺序的评估分数
          double score = Evaluate(new_order, adjacency_list, edges);

          // 将新顶点顺序和评估分数添加到后继状态中
          successors.push_back({new_order, score});
        }
      }
    }
  }

  // 选择评估分数最小的 top-K 后继状态
  sort(successors.begin(), successors.end());
  if (successors.size() > beam_width) {
    successors.resize(beam_width);
  }

  return successors;
}

vector<int> CalculateGoodVertexOrder(const vector<int>& start_vertices,
                                     const vector<vector<int>>& adjacency_list,
                                     int beam_width, int iterations,
                                     const vector<Edge>& edges) {
  priority_queue<State, vector<State>, greater<State>> beam;

  // 打印每一轮迭代下的顶点顺序，以及在当前顶点顺序下的评估函数值
  // int cnt_iterations = 1;

  // 初始化Beam Search的起始状态
  for (auto& start_vertice : start_vertices) {
    beam.push({{start_vertice}, 0.0});
  }

  while (!beam.empty() && iterations > 0) {
    priority_queue<State, vector<State>, greater<State>> next_beam;

    while (!beam.empty()) {
      State current = beam.top();
      beam.pop();

      if (current.vertex_order.size() == adjacency_list.size() - 1) {
        // 达到终止条件，返回当前最佳顶点顺序
        return current.vertex_order;
      }

      // 生成后继状态
      vector<State> successors =
          GenerateSuccessors(current, adjacency_list, beam_width, edges);

      for (const State& successor : successors) {
        next_beam.push(successor);
      }
    }

    // cout << "第" << cnt_iterations << "轮迭代结果:" << endl;

    // 选择优秀的后继状态，准备下一轮迭代
    for (int i = 0; i < beam_width && !next_beam.empty(); ++i) {
      beam.push(next_beam.top());

      // // 打印每一轮迭代中的结果
      // cout << "结果" << i + 1 << ":";
      // for(auto& vertex : next_beam.top().vertex_order) {
      //     cout << vertex << " ";
      // }
      // cout << "得分:" << next_beam.top().score << endl;

      next_beam.pop();
    }

    // cnt_iterations++;

    iterations--;
  }

  // 返回最佳顶点顺序
  return beam.top().vertex_order;
}

// 将顶点顺序转化为边的顺序
vector<Edge> CalculateEdgeOrder(const vector<int>& good_vertex_order,
                                const vector<Edge>& edges) {
  // 将边按照顶点顺序的前沿区间的交集进行排序
  vector<Edge> sorted_edges = edges;
  sort(sorted_edges.begin(), sorted_edges.end(),
       [&](const Edge& e1, const Edge& e2) {
         // 获取边e1和e2的相关顶点在good_vertex_order中的索引
         int idx_e1_u = distance(good_vertex_order.begin(),
                                 find(good_vertex_order.begin(),
                                      good_vertex_order.end(), e1.first));
         int idx_e1_v = distance(good_vertex_order.begin(),
                                 find(good_vertex_order.begin(),
                                      good_vertex_order.end(), e1.second));
         int idx_e2_u = distance(good_vertex_order.begin(),
                                 find(good_vertex_order.begin(),
                                      good_vertex_order.end(), e2.first));
         int idx_e2_v = distance(good_vertex_order.begin(),
                                 find(good_vertex_order.begin(),
                                      good_vertex_order.end(), e2.second));

         // 计算边e1和e2相关顶点的前沿区间的交集的大小
         // int intersection_e1_left = min(idx_e1_u, idx_e1_v);
         // int intersection_e2_left = min(idx_e2_u, idx_e2_v);

         // 计算边e1和e2相关顶点的前沿区间的交集的大小
         int intersection_e1_right = max(idx_e1_u, idx_e1_v);
         int intersection_e2_right = max(idx_e2_u, idx_e2_v);

         // 将边按照前沿区间的交集大小进行排序
         // if(intersection_e1_right!=intersection_e2_right)
         //     return intersection_e1_right < intersection_e2_right;
         // else
         //     return intersection_e1_left < intersection_e2_left;

         return intersection_e1_right < intersection_e2_right;
       });

  return sorted_edges;
}

void ConstructVtree(vector<vector<int>>& Vtree,
                    const vector<int>& good_vertex_order,
                    const vector<Edge>& edge_order) {
  int pre_ei_right = -1;
  for (int i = 0; i < edge_order.size(); i++) {
    int idx_ei_u = distance(good_vertex_order.begin(),
                            find(good_vertex_order.begin(),
                                 good_vertex_order.end(), edge_order[i].first));
    int idx_ei_v =
        distance(good_vertex_order.begin(),
                 find(good_vertex_order.begin(), good_vertex_order.end(),
                      edge_order[i].second));
    int ei_right = max(idx_ei_u, idx_ei_v);
    if (ei_right != pre_ei_right) {
      Vtree.push_back(vector<int>{i});
    } else {
      Vtree.back().push_back(i);
    }
    pre_ei_right = ei_right;
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cerr << "Usage: " << argv[0] << " <input_file_path>" << endl;
    return 1;
  }
  // 通过命令行参数获取文件路径
  string file_path = argv[1];
  int lastSlashIndex = file_path.rfind('/');
  std::string file_name = file_path.substr(lastSlashIndex + 1);

  // 通过类来读取文件中的边
  Graph G;
  if (G.readfromFile(file_path.c_str())) {
    cout << "读取" << file_name << "成功！" << endl;
  } else {
    cout << "读取" << file_name << "失败！" << endl;
  }

  // 取得类中的边集存入edges中
  vector<Edge> edges = G.e;
  int max_vertex = G.n;

  // 将vector<Edge> edges转化为邻接列表
  vector<vector<int>> adjacency_list =
      ConvertToAdjacencyList(edges, max_vertex);

  // 设置Beam Search的束宽和迭代次数
  int beam_width = 50;
  int iterations = 100000;

  // 计算一个好的起始顶点顺序
  vector<int> good_start_vertices =
      CalculateAppropriateStartVertices(max_vertex, adjacency_list, edges);

  // 打印good_start_vertices
  // cout << "good_start_vertices: ";
  // for (int vertex : good_start_vertices) {
  //     cout << vertex << " ";
  // }
  // cout << endl;

  // 取其前L个元素作为Beam Search的起始顶点，根据方法Beam Search计算的顶点顺序
  int L = good_start_vertices.size() <= 10 ? good_start_vertices.size() : 10;
  vector<int> start_vertices;
  start_vertices.assign(good_start_vertices.begin(),
                        good_start_vertices.begin() + L);

  // 取其第一个元素作为起始顶点，根据线性时间启发式方法计算的顶点顺序
  int start_vertex = good_start_vertices[0];

  // 统计分别使用两种方法后程序的运行时间
  auto start = std::chrono::system_clock::now();

  // 方法一：Beam Search，并计算最终确定的顶点顺序的评分
  // vector<int> good_vertex_order1 = CalculateGoodVertexOrder(
  //     start_vertices, adjacency_list, beam_width, iterations, edges);
  // double score1 = Evaluate(good_vertex_order1, adjacency_list, edges);

  // 方法二：线性时间启发式方法，并计算最终确定的顶点顺序的评分
  // vector<int> good_vertex_order2 =
  //     GetGoodVertexOrder(start_vertex, adjacency_list);
  vector<int> good_vertex_order2 =
      GetGoodVertexOrder(start_vertex, adjacency_list);
  double score2 = Evaluate(good_vertex_order2, adjacency_list, edges);

  auto end = std::chrono::system_clock::now();

  // 只打印方法二的运行结果
  double ptime_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();
  double ptime_s = ptime_ms / 1000.0;
  cout << "运行时间为:";
  std::cout << std::fixed << std::setprecision(6) << ptime_s << " s"
            << std::endl;
  cout << "得分：" << score2 << endl;
  vector<int> good_vertex_order;
  good_vertex_order = good_vertex_order2;

  // 打印方法一和方法二的运行结果
  // double ptime_ms =
  //     std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
  //         .count();
  // double ptime_s = ptime_ms / 1000.0;
  // cout << "运行时间为:";
  // std::cout << std::fixed << std::setprecision(6) << ptime_s << " s"
  //           << std::endl;
  // cout << "得分：";
  // cout << "Beam Search: " << score1 << " "
  //      << "线性时间启发式方法: " << score2 << endl;
  // vector<int> good_vertex_order;
  // if (score1 <= score2) {
  //   cout << "Beam Search产生更好的结果" << endl;
  //   good_vertex_order = good_vertex_order1;
  // } else {
  //   cout << "线性时间启发式方法产生更好的结果" << endl;
  //   good_vertex_order = good_vertex_order2;
  // }

  // 打印Good Vertex Order
  // cout << "Good Vertex Order: ";
  // for (int vertex : good_vertex_order) {
  //   cout << vertex << " ";
  // }
  // cout << endl;

  // 将顶点顺序转化为边的顺序
  vector<Edge> edge_order = CalculateEdgeOrder(good_vertex_order, edges);

  // 根据最终的边顺序确定构造ZSDD的Vtree
  // vector<vector<int>> Vtree;
  // ConstructVtree(Vtree, good_vertex_order, edge_order);

  // 打印该Vtree
  //   cout << "Vtree:" << endl;
  //   for (auto& edges : Vtree) {
  //     cout << "[";
  //     for (auto& edge : edges) {
  //       cout << edge << " ";
  //     }
  //     cout << "]" << endl;
  //   }

  // 打开输出文件流
  string output_file_name = argv[2];
  ofstream outfile(output_file_name);

  // 将结果写入输出文件
  for (const auto& edge : edge_order) {
    outfile << edge.first << " " << edge.second << endl;
  }
  // 关闭输出文件流
  outfile.close();
  cout << "Edge order has been written to ./output_graph/" << endl;
  cout << "-----------------------------------------------end" << endl;
  return 0;
}