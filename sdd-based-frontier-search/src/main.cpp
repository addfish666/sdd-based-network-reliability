#include "network.hpp"
#include "state.hpp"
using namespace std;
using namespace tdzdd;

int main(int argc, char *argv[]) {
  /*******************************************
  网络图数据输入
  *******************************************/
  tdzdd::Graph graph;
  read_network(graph, argv[1], argv[2]);

  State::fbs = new FrontierBasedSearch(graph, -1, false, false);
  State s_initial;

  SddLiteral var_count = graph.edgeSize();
  const char *type = "right";
  Vtree *vtree = sdd_vtree_new(var_count, type);
  SddManager *manager = sdd_manager_new(vtree);

  printf("constructing SDD ... ");
  SddNode *node = construct(s_initial, vtree, manager);
  printf("done\n");

  printf("saving sdd and vtree ... ");
  sdd_save_as_dot("output/sdd.dot", node);
  sdd_vtree_save_as_dot("output/vtree.dot", vtree);
  printf("done\n");

  return 0;
}