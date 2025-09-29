#include <bits/stdc++.h>

template <class T, class Comp = std::less<T>>
class BinaryTree {
private:
  struct Node {
    T value;
    std::unique_ptr<Node> left, right;
    explicit Node(const T& v) : value(v) {}
    explicit Node(T&& v) : value(std::move(v)) {}
    ~Node() = default;
  };

  std::unique_ptr<Node> root_;
  std::size_t size_ = 0;
  Comp comp_{};

public:
  BinaryTree() = default;
  ~BinaryTree() = default;

  [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
  [[nodiscard]] std::size_t size() const noexcept { return size_; }

  void insert(const T& v) { insert_impl(root_, v); }
  void insert(T&& v) { insert_impl(root_, std::move(v)); }

  [[nodiscard]] bool contains(const T& x) const {
    const Node* cur = root_.get();
    while (cur) {
      if (comp_(x, cur->value)) cur = cur->left.get();
      else if (comp_(cur->value, x)) cur = cur->right.get();
      else return true;
    }
    return false;
  }

  [[nodiscard]] int height() const noexcept { return height_impl(root_.get()); }

  [[nodiscard]] std::vector<T> preorder() const {
    std::vector<T> a; a.reserve(size_); preorder_collect(root_.get(), a); return a;
  }
  [[nodiscard]] std::vector<T> inorder() const {
    std::vector<T> a; a.reserve(size_); inorder_collect(root_.get(), a); return a;
  }
  [[nodiscard]] std::vector<T> postorder() const {
    std::vector<T> a; a.reserve(size_); postorder_collect(root_.get(), a); return a;
  }

  template <class F>
  void preorder(F f) const { preorder_visit(root_.get(), f); }
  template <class F>
  void inorder(F f) const { inorder_visit(root_.get(), f); }
  template <class F>
  void postorder(F f) const { postorder_visit(root_.get(), f); }

private:
  template <class U>
  void insert_impl(std::unique_ptr<Node>& node, U&& v) {
    if (!node) { node = std::make_unique<Node>(std::forward<U>(v)); ++size_; return; }
    if (comp_(v, node->value)) insert_impl(node->left, std::forward<U>(v));
    else if (comp_(node->value, v)) insert_impl(node->right, std::forward<U>(v));
    else { }
  }

  static int height_impl(const Node* n) noexcept {
    if (!n) return -1;
    return 1 + std::max(height_impl(n->left.get()), height_impl(n->right.get()));
  }

  static void preorder_collect(const Node* n, std::vector<T>& a) {
    if (!n) return;
    a.push_back(n->value);
    preorder_collect(n->left.get(), a);
    preorder_collect(n->right.get(), a);
  }
  static void inorder_collect(const Node* n, std::vector<T>& a) {
    if (!n) return;
    inorder_collect(n->left.get(), a);
    a.push_back(n->value);
    inorder_collect(n->right.get(), a);
  }
  static void postorder_collect(const Node* n, std::vector<T>& a) {
    if (!n) return;
    postorder_collect(n->left.get(), a);
    postorder_collect(n->right.get(), a);
    a.push_back(n->value);
  }

  template <class F>
  static void preorder_visit(const Node* n, F& f) {
    if (!n) return;
    f(n->value);
    preorder_visit(n->left.get(), f);
    preorder_visit(n->right.get(), f);
  }
  template <class F>
  static void inorder_visit(const Node* n, F& f) {
    if (!n) return;
    inorder_visit(n->left.get(), f);
    f(n->value);
    inorder_visit(n->right.get(), f);
  }
  template <class F>
  static void postorder_visit(const Node* n, F& f) {
    if (!n) return;
    postorder_visit(n->left.get(), f);
    postorder_visit(n->right.get(), f);
    f(n->value);
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  BinaryTree<int> bt;
  for (int x : {7, 3, 9, 1, 5, 8, 10, 4, 6}) bt.insert(x);

  std::cout << "size=" << bt.size() << " height=" << bt.height() << "\n";

  auto in = bt.inorder();
  std::cout << "Inorder: ";
  for (int x : in) std::cout << x << ' ';
  std::cout << "\n";

  std::cout << "Preorder (callback): ";
  bt.preorder([](const int& x){ std::cout << x << ' '; });
  std::cout << "\n";

  std::cout << std::boolalpha << "contains 5? " << bt.contains(5) << ", contains 42? " << bt.contains(42) << "\n";
  
  return 0;
}
