#include <iostream>
#include <vector>

struct TNode {
  int val;
  TNode* left;
  TNode* right;
  bool lthread;
  bool rthread;
  TNode(int v): val(v), left(nullptr), right(nullptr), lthread(false), rthread(false) {}
};

TNode* bst_insert(TNode* root, int v) {
  if (!root) return new TNode(v);
  TNode* cur = root;
  while (true) {
    if (v < cur->val) {
      if (!cur->left || cur->lthread) {
        if (cur->lthread) { cur->left = nullptr; cur->lthread = false; }
        if (!cur->left) { cur->left = new TNode(v); break; }
      }
      cur = cur->left;
    } else if (v > cur->val) {
      if (!cur->right || cur->rthread) {
        if (cur->rthread) { cur->right = nullptr; cur->rthread = false; }
        if (!cur->right) { cur->right = new TNode(v); break; }
      }
      cur = cur->right;
    } else break;
  }
  return root;
}

void inorder_thread_dfs(TNode* x, TNode*& prev) {
  if (!x) return;
  if (!x->lthread) inorder_thread_dfs(x->left, prev);
  if (!x->left) { x->lthread = true; x->left = prev; }
  if (prev && !prev->right) { prev->rthread = true; prev->right = x; }
  prev = x;
  if (!x->rthread) inorder_thread_dfs(x->right, prev);
}

void inorder_thread(TNode* root) {
  TNode* prev = nullptr;
  inorder_thread_dfs(root, prev);
}

TNode* leftmost(TNode* x) {
  if (!x) return nullptr;
  while (!x->lthread && x->left) x = x->left;
  return x;
}

std::vector<int> inorder_traverse_threaded(TNode* root) {
  std::vector<int> seq;
  for (TNode* cur = leftmost(root); cur; ) {
    seq.push_back(cur->val);
    if (cur->rthread) cur = cur->right;
    else cur = leftmost(cur->right);
  }
  return seq;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  std::vector <int> a = {8, 3, 10, 1, 6, 14, 4, 7, 13};
  TNode* root = nullptr;
  for (int v : a) root = bst_insert(root, v);
  inorder_thread(root);
  auto seq = inorder_traverse_threaded(root);
  for (int v : seq) std::cout << v << ' ';
  std::cout << '\n';

  return 0;
}
