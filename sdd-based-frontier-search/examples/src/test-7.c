#include <stdio.h>
#include <stdlib.h>

#include "sddapi.h"

SddNode *construct_sdd_with_truth_assignment(const SddLiteral *var_order,
                                             int var_order_size,
                                             const int *truth_assignment,
                                             int truth_assignment_size,
                                             SddManager *manager) {
  SddNode *sdd = sdd_manager_true(manager);
  for (int i = 0; i < truth_assignment_size; i++) {
    if (truth_assignment[i] == 1) {
      SddLiteral literal = *var_order++;
      sdd = sdd_conjoin(sdd, sdd_manager_literal(literal, manager), manager);
    } else {
      SddLiteral literal = *var_order++;
      literal = -literal;
      sdd = sdd_conjoin(sdd, sdd_manager_literal(literal, manager), manager);
    }
  }
  return sdd;
}

int main(int argc, char **argv) {
  // set up vtree and manager
  SddLiteral var_count = 6;
  const char *type = "balanced";

  Vtree *vtree = sdd_vtree_new(var_count, type);
  SddManager *manager = sdd_manager_new(vtree);

  Vtree *vtree_left = sdd_vtree_left(vtree);
  SddLiteral var_order_size = sdd_vtree_var_count(vtree_left);
  SddLiteral var_order[var_order_size];
  // SddLiteral var_order_first = sdd_vtree_var(sdd_vtree_first(vtree_left));
  // for (int i = 0; i < var_order_size; ++i) {
  //   var_order[i] = var_order_first++;
  // }
  sdd_vtree_var_order(var_order, vtree_left);
  printf("Var Order: ");
  for (int i = 0; i < var_order_size; ++i) {
    printf("%ld ", var_order[i]);
  }
  printf("\n");

  int truth_assignment_size = 3;
  int truth_assignment[truth_assignment_size];
  truth_assignment[0] = 0;
  truth_assignment[1] = 1;
  truth_assignment[2] = 1;

  printf("constructing SDD ... ");
  SddNode *sdd = construct_sdd_with_truth_assignment(
      var_order, var_order_size, truth_assignment, truth_assignment_size,
      manager);
  printf("done\n");

  printf("saving sdd and vtree ... ");
  sdd_save_as_dot("output/sdd1.dot", sdd);
  sdd_vtree_save_as_dot("output/vtree.dot", vtree);
  printf("done\n");

  sdd_vtree_free(vtree);
  sdd_manager_free(manager);

  return 0;
}
