#include "../include/state.hpp"

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