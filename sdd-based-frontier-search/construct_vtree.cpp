#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
// control strings
#define PRIsS "zu"
#define PRInsS "u"
#define PRIrcS "u"
#define PRImcS "llu"
#define PRIwmcS "f"
#define PRIlitS "ld"

class vtree {
 public:
  vtree* left;
  vtree* right;
  int position;
  int var;  // 当var为0是代表内部节点
  vtree(int position, int var)
      : left(nullptr), right(nullptr), position(position), var(var) {}
  vtree(vtree* left, vtree* right, int position, int var)
      : left(left), right(right), position(position), var(var) {}
};

vtree* right_linear_vtree(vector<int>& group_array, int start);
vtree* balanced_vtree(int l, int r);
int position = 0;
int var = 1;

vtree* right_linear_vtree(vector<int>& group_array, int start) {
  // base case
  vtree* node = new vtree(0, 0);
  if (start == group_array.size() - 1) {
    return group_array[start] > 1 ? balanced_vtree(0, group_array[start] - 1)
                                  : node;
  }

  node->left = balanced_vtree(0, group_array[start] - 1);
  node->right = right_linear_vtree(group_array, start + 1);
  return node;
}

vtree* balanced_vtree(int l, int r) {
  // base case
  if (l > r) return nullptr;
  vtree* node = new vtree(0, 0);
  if (l == r) return node;

  int m = (l + r) / 2;
  node->left = balanced_vtree(l, m);
  node->right = balanced_vtree(m + 1, r);
  return node;
}

void set_position_and_var(vtree* node) {
  if (!node) return;
  set_position_and_var(node->left);
  if (!node->left && !node->right) node->var = var++;
  node->position = position++;
  set_position_and_var(node->right);
}

void print_vtree_node(FILE* file, const vtree* vnode) {
  if (vnode->var != 0) {
    fprintf(file, "L %" PRIsS " %" PRIlitS "", vnode->position, vnode->var);
  } else {  // internal node
    print_vtree_node(file, vnode->left);
    print_vtree_node(file, vnode->right);
    fprintf(file, "I %" PRIsS " %" PRIsS " %" PRIsS "", vnode->position,
            vnode->left->position, vnode->right->position);
  }
  fprintf(file, "\n");
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    cerr << "Usage: " << argv[0] << " <graph_name> <edge_count> <arg_m>"
         << endl;
    return 1;
  }

  // arg_m:为vtree的参数m
  int arg_m = atoi(argv[3]);
  int m = atoi(argv[2]);
  int x1 = m / arg_m;
  int x2 = m % arg_m;
  cout << "x1: " << x1 << " x2:" << x2 << endl;
  vector<int> group_size;
  if (x2 != 0) {
    group_size = vector<int>(x1 + 1, arg_m);
    group_size[x1] = x2;
  } else {
    group_size = vector<int>(x1, arg_m);
  }
  vtree* node = right_linear_vtree(group_size, 0);
  set_position_and_var(node);

  string folderPath = "vtree/";
  string filePath = folderPath + argv[1] + ".vtree";
  FILE* file = fopen(filePath.c_str(), "w");

  int sum = 0;
  for (auto& ele : group_size) sum += ele;
  int count = 2 * sum - 1;
  fprintf(file, "vtree %d\n", count);

  print_vtree_node(file, node);
  fclose(file);
  return 0;
}