#ifndef SDD_HPP
#define SDD_HPP

#include <unordered_map>

#include "../tdzdd/DdSpec.hpp"
#include "../tdzdd/DdStructure.hpp"
#include "../tdzdd/spec/FrontierBasedSearch.hpp"
#include "../tdzdd/util/MyVector.hpp"
#include "sddapi.h"

using namespace tdzdd;

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

SddNode *construct_sdd_with_truth_assignment(
    const SddLiteral *var_order, int var_order_size,
    const MyVector<bool> truth_assignment, SddManager *manager);

SddNode *construct(State s, Vtree *vtree, SddManager *manager);

SddNode *construct_dec_node(State s, Vtree *vtree, const SddLiteral *var_order,
                            int var_order_size, int i,
                            MyVector<bool> truth_assignment,
                            SddManager *manager);

void construct_dec_node_auxiliary(
    State s, Vtree *vtree, const SddLiteral *var_order, int var_order_size,
    int i, MyVector<bool> truth_assignment, SddManager *manager,
    void fn(SddNode *prime, SddNode *sub, Vtree *, SddManager *manager));

#endif  // SDD_HPP