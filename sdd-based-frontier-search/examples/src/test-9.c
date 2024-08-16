#include <stdio.h>
#include <stdlib.h>

#include "sddapi.h"

int main(int argc, char** argv) {
  // set up vtree and manager
  Vtree* vtree = sdd_vtree_read("input/rotate-left.vtree");
  SddManager* manager = sdd_manager_new(vtree);

  printf("saving vtree & sdd ...\n");
  sdd_vtree_save_as_dot("output/vtree.dot", vtree);

  sdd_vtree_free(vtree);
  sdd_manager_free(manager);

  return 0;
}
