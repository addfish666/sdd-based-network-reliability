#include <stdio.h>
#include <stdlib.h>

#include "sddapi.h"

int main(int argc, char **argv) {
  // set up vtree and manager
  SddLiteral var_count = 6;
  const char *type = "balanced";
  SddLiteral A = 1, B = 2, C = 3, D = 4, E = 5, F = 6;

  Vtree *vtree = sdd_vtree_new(var_count, type);
  SddManager *manager = sdd_manager_new(vtree);

  Vtree *vtree_right = sdd_vtree_right(vtree);
  SddLiteral array_size = sdd_vtree_var_count(vtree_right);
  SddLiteral var_order[array_size];
  sdd_vtree_var_order(var_order, vtree_right);
  printf("Var Order: ");
  for (int i = 0; i < array_size; ++i) {
    printf("%ld ", var_order[i]);
  }
  printf("\n");

  // construct a formula (A^B)v(B^C)v(C^D)
  printf("constructing SDD ... ");
  SddNode *f_a = sdd_manager_literal(A, manager);
  SddNode *f_b = sdd_manager_literal(B, manager);
  SddNode *f_c = sdd_manager_literal(C, manager);
  SddNode *f_d = sdd_manager_literal(D, manager);
  SddNode *f_e = sdd_manager_literal(E, manager);
  SddNode *f_f = sdd_manager_literal(F, manager);

  SddNode *_f_a = sdd_manager_literal(-A, manager);
  SddNode *_f_b = sdd_manager_literal(-B, manager);
  SddNode *_f_c = sdd_manager_literal(-C, manager);
  SddNode *_f_d = sdd_manager_literal(-D, manager);
  SddNode *_f_e = sdd_manager_literal(-E, manager);
  SddNode *_f_f = sdd_manager_literal(-F, manager);

  SddNode *s1 = sdd_conjoin(f_a, f_b, manager);
  s1 = sdd_conjoin(s1, f_c, manager);

  SddNode *s2 = sdd_conjoin(f_a, f_b, manager);
  s2 = sdd_conjoin(s2, _f_c, manager);

  SddNode *s3 = sdd_disjoin(s1, s2, manager);

  SddNode *s4 = sdd_conjoin(f_d, f_e, manager);
  s4 = sdd_conjoin(s4, f_f, manager);

  SddNode *s5 = sdd_conjoin(f_d, f_e, manager);
  s5 = sdd_conjoin(s5, _f_f, manager);

  SddNode *s6 = sdd_disjoin(s4, s5, manager);
  printf("done\n");

  printf("saving sdd and vtree ... ");
  sdd_save_as_dot("output/sdd1.dot", s1);
  sdd_save_as_dot("output/sdd2.dot", s2);
  sdd_save_as_dot("output/sdd3.dot", s3);
  sdd_save_as_dot("output/sdd4.dot", s4);
  sdd_save_as_dot("output/sdd5.dot", s5);
  sdd_save_as_dot("output/sdd6.dot", s6);
  sdd_vtree_save_as_dot("output/vtree.dot", vtree);
  printf("done\n");

  sdd_vtree_free(vtree);
  sdd_manager_free(manager);

  return 0;
}
