#include <stdio.h>
#include <stdlib.h>

#include <ctime>
#include <unordered_map>
#include <unordered_set>

#include "../../tdzdd/DdSpec.hpp"
#include "../../tdzdd/DdStructure.hpp"
#include "../../tdzdd/spec/FrontierBasedSearch.hpp"
#include "../../tdzdd/util/MyVector.hpp"
#include "../tdzdd/util/Graph.hpp"
#include "../tdzdd/util/MessageHandler.hpp"

using namespace std;
using namespace tdzdd;

extern "C" {
#include "sddapi.h"
}

class State {
 public:
  /*
  -1:all vertex of T is connected in the graph
  0:one vertex of T is disconnected in the graph
  others:代表处理到第几条边
  */
  int level;
  MyVector<int64_t> state;
  static FrontierBasedSearch *fbs;

 public:
  State() {
    state = MyVector<int64_t>(fbs->datasize());
    int n = fbs->get_root(state.data());
    level = n;
  }
  State(int _level, MyVector<int64_t> _state, int t) {
    state = MyVector<int64_t>(fbs->datasize());
    fbs->get_copy(state.data(), _state.data());
    level = fbs->get_child(state.data(), _level, t);
  }
  ~State() { state.clear(); }
  size_t hashKey() { return fbs->hash_code(state.data(), level); }
  void print() {
    std::cout << "level:" << level << " ";
    fbs->print_state(std::cout, state.data(), level);
  }
};

FrontierBasedSearch *State::fbs = nullptr;

std::unordered_map<size_t, SddNode *> state_to_sdd;

std::unordered_map<SddSize, double> sddid_to_reliablility;

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

SddNode *construct(State s, Vtree *vtree, SddManager *manager);
void construct_dec_node_auxiliary(
    State s, Vtree *vtree, const SddLiteral *var_order, int var_order_size,
    int i, MyVector<bool> truth_assignment, SddManager *manager,
    void fn(SddNode *prime, SddNode *sub, Vtree *, SddManager *manager));

void read_network(tdzdd::Graph &graph, const char *filename_graph,
                  const char *filename_terminal) {
  Network G;  // 确定BDD变量顺序后的网络图
  G.readfromFile(filename_graph);

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

SddNode *construct_sdd_with_truth_assignment(
    const SddLiteral *var_order, int var_order_size,
    const MyVector<bool> truth_assignment, SddManager *manager) {
  int truth_assignment_size = truth_assignment.size();
  SddNode *node = sdd_manager_true(manager);
  for (int i = 0; i < truth_assignment_size; i++) {
    SddLiteral literal = *var_order++;
    if (truth_assignment[i]) {
      node = sdd_conjoin(node, sdd_manager_literal(literal, manager), manager);
    } else {
      literal = -literal;
      node = sdd_conjoin(node, sdd_manager_literal(literal, manager), manager);
    }
  }
  return node;
}

SddNode *construct_dec_node(State s, Vtree *vtree, const SddLiteral *var_order,
                            int var_order_size, int i,
                            MyVector<bool> truth_assignment,
                            SddManager *manager) {
  SddNode *node;
  START_partition(manager);
  construct_dec_node_auxiliary(s, vtree, var_order, var_order_size, i,
                               truth_assignment, manager, DECLARE_element);
  node = GET_node_of_partition(vtree, manager, 0);
  return node;
}

void construct_dec_node_auxiliary(
    State s, Vtree *vtree, const SddLiteral *var_order, int var_order_size,
    int i, MyVector<bool> truth_assignment, SddManager *manager,
    void fn(SddNode *prime, SddNode *sub, Vtree *, SddManager *manager)) {
  if (i > var_order_size) {
    SddNode *prime = construct_sdd_with_truth_assignment(
        var_order, var_order_size, truth_assignment, manager);
    SddNode *sub;
    size_t key = s.hashKey();
    if (state_to_sdd.count(key)) {
      sub = state_to_sdd[key];
    } else {
      sub = construct(s, sdd_vtree_right(vtree), manager);
    }
    fn(prime, sub, vtree, manager);
    return;
  }

  State s_1(s.level, s.state, 1);
  truth_assignment.push_back(true);
  if (s_1.level == -1) {
    SddNode *prime = construct_sdd_with_truth_assignment(
        var_order, var_order_size, truth_assignment, manager);
    SddNode *sub = sdd_manager_true(manager);

    fn(prime, sub, vtree, manager);

  } else if (s_1.level == 0) {
    SddNode *prime = construct_sdd_with_truth_assignment(
        var_order, var_order_size, truth_assignment, manager);
    SddNode *sub = sdd_manager_false(manager);

    fn(prime, sub, vtree, manager);

  } else {
    construct_dec_node_auxiliary(s_1, vtree, var_order, var_order_size, i + 1,
                                 truth_assignment, manager, DECLARE_element);
  }
  truth_assignment.pop_back();

  // 往E_s中去除当前正在处理的边
  State s_0(s.level, s.state, 0);
  truth_assignment.push_back(false);
  if (s_0.level == 0) {
    SddNode *prime = construct_sdd_with_truth_assignment(
        var_order, var_order_size, truth_assignment, manager);
    SddNode *sub = sdd_manager_false(manager);

    fn(prime, sub, vtree, manager);

  } else if (s_0.level == -1) {
    SddNode *prime = construct_sdd_with_truth_assignment(
        var_order, var_order_size, truth_assignment, manager);
    SddNode *sub = sdd_manager_true(manager);

    fn(prime, sub, vtree, manager);

  } else
    construct_dec_node_auxiliary(s_0, vtree, var_order, var_order_size, i + 1,
                                 truth_assignment, manager, DECLARE_element);
  truth_assignment.pop_back();
}

SddNode *construct(State s, Vtree *vtree, SddManager *manager) {
  SddNode *node;
  size_t key = s.hashKey();
  if (state_to_sdd.count(key)) {
    node = state_to_sdd[key];
    return node;
  }
  if (sdd_vtree_is_leaf(vtree)) {
    State s_0(s.level, s.state, 0);
    if (s_0.level == -1) {
      node = sdd_manager_true(manager);
    } else {
      node = sdd_manager_literal(sdd_vtree_var(vtree), manager);
    }
    return node;
  } else {
    // 获取当前vtree左子树叶子节点所代表的变量集合
    Vtree *vtree_left = sdd_vtree_left(vtree);
    SddLiteral var_order_size = sdd_vtree_var_count(vtree_left);
    SddLiteral var_order[var_order_size];
    sdd_vtree_var_order(var_order, vtree_left);

    MyVector<bool> truth_assignment;
    truth_assignment.reserve(var_order_size);

    node = construct_dec_node(s, vtree, var_order, var_order_size, 1,
                              truth_assignment, manager);
    state_to_sdd[key] = node;
    return node;
  }
}

// double calculate_reliablility_through_sdd(SddNode *node,
//                                           const std::vector<double> &p) {
//   // base case
//   if (sdd_node_is_true(node)) return 1.0;
//   if (sdd_node_is_false(node)) return 0.0;
//   if (sdd_node_is_literal(node)) {
//     SddLiteral literal = sdd_node_literal(node);
//     int index = literal > 0 ? literal : -literal;
//     index--;
//     return literal > 0 ? p[index] : (1.0 - p[index]);
//   }

//   // must recurse
//   SddNode **elems = sdd_node_elements(node);
//   SddNodeSize elems_size = sdd_node_size(node);
//   double res = 0.0;
//   for (SddNode **elem = elems; elem < elems + elems_size * 2; elem += 2) {
//     SddNode *prime = *elem;
//     SddNode *sub = *(elem + 1);
//     res += calculate_reliablility_through_sdd(prime, p) *
//            calculate_reliablility_through_sdd(sub, p);
//   }
//   return res;
// }

double calculate_reliablility_through_sdd(SddNode *node,
                                          const std::vector<double> &p) {
  // base case
  if (sdd_node_is_true(node)) return 1.0;
  if (sdd_node_is_false(node)) return 0.0;
  if (sdd_node_is_literal(node)) {
    SddLiteral literal = sdd_node_literal(node);
    int index = literal > 0 ? literal : -literal;
    index--;
    return literal > 0 ? p[index] : (1.0 - p[index]);
  }

  // must recurse
  SddNode **elems = sdd_node_elements(node);
  SddNodeSize elems_size = sdd_node_size(node);
  double res = 0.0;
  for (SddNode **elem = elems; elem < elems + elems_size * 2; elem += 2) {
    SddNode *prime = *elem;
    SddNode *sub = *(elem + 1);
    // 提前剪支
    if (sdd_node_is_false(sub)) {
      continue;
    }
    double prime_reliablility = 0.0;
    double sub_reliablility = 0.0;
    if (sddid_to_reliablility.count(sdd_id(prime)))
      prime_reliablility = sddid_to_reliablility[sdd_id(prime)];
    else
      prime_reliablility = calculate_reliablility_through_sdd(prime, p);

    if (sddid_to_reliablility.count(sdd_id(sub)))
      sub_reliablility = sddid_to_reliablility[sdd_id(sub)];
    else
      sub_reliablility = calculate_reliablility_through_sdd(sub, p);

    res += prime_reliablility * sub_reliablility;
  }
  sddid_to_reliablility[sdd_id(node)] = res;
  return res;
}

int main(int argc, char *argv[]) {
  /*******************************************
  网络图数据输入
  *******************************************/
  tdzdd::Graph graph;
  read_network(graph, argv[1], argv[2]);

  // 初始化概率数组
  // vector<double> p(graph.edgeSize(), 0.9);
  vector<double> p = {0.8, 0.7, 0.6, 0.5, 0.4, 0.3};
  // vector<double> p = {0.7, 0.8, 0.6, 0.4, 0.5, 0.3};

  State::fbs = new FrontierBasedSearch(graph, -1, false, false);
  State s_initial;

  // SddLiteral var_count = graph.edgeSize();
  // const char *type = "random";
  // Vtree *vtree = sdd_vtree_new(var_count, type);
  Vtree *vtree = sdd_vtree_read("input/example.vtree");
  SddManager *manager = sdd_manager_new(vtree);

  const char *network =
      strstr(argv[1], "real-network/") + strlen("real-network/");
  printf("Network: %s\n", network);

  printf("constructing SDD ... ");
  clock_t start_time = clock();
  SddNode *node = construct(s_initial, vtree, manager);
  clock_t end_time = clock();
  printf("done\n");

  double elapsed_time =
      static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC;
  printf("Elapsed Time: %.4f seconds\n", elapsed_time);

  printf("saving sdd and vtree ... ");
  sdd_save_as_dot("output/sdd.dot", node);
  sdd_vtree_save_as_dot("output/vtree.dot", vtree);
  printf("done\n");

  SddSize size = sdd_size(node);
  printf("SDD Size: %zu\n", size);

  SddSize count = sdd_count(node);
  printf("SDD count: %zu\n", count);

  int n = state_to_sdd.size();
  printf("SDD hash table size: %d\n", n);

  double res = calculate_reliablility_through_sdd(node, p);
  printf("reliablility: %f\n\n", res);

  int i = 1;
  SddNode **elems = sdd_node_elements(node);
  SddNodeSize elems_size = sdd_node_size(node);
  for (SddNode **elem = elems; elem < elems + elems_size * 2; elem += 2) {
    SddNode *prime = *elem;
    SddNode *sub = *(elem + 1);
    printf("i = %d\n", i);
    printf("reliablility: %f\n\n",
           calculate_reliablility_through_sdd(prime, p));
    printf("reliablility: %f\n\n", calculate_reliablility_through_sdd(sub, p));
    printf("-------------\n");
    i++;
  }

  // 释放内存
  sdd_manager_free(manager);
  delete State::fbs;

  return 0;
}