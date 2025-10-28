#include <memory>
#include <utility>
#include <stdexcept>
#include <concepts>
#include <functional>
#include <cstddef>
#include <type_traits>
#include <ranges>

template <class T, class Compare = std::less<T>>
requires std::strict_weak_order<Compare, const T&, const T&>
class LeftistHeap {
private:
  struct Node {
    T key;
    int npl = 1;
    std::unique_ptr<Node> left{}, right{};
    explicit Node(T v) : key(std::move(v)) {}
  };

  std::unique_ptr<Node> root{};
  std::size_t count_ = 0;
  [[no_unique_address]] Compare comp{};

  static int nplOf(const std::unique_ptr<Node>& p) noexcept { return p ? p->npl : 0; }

  static std::unique_ptr<Node> mergeNodes(std::unique_ptr<Node> a, std::unique_ptr<Node> b, Compare comp) {
    if (!a) return b;
    if (!b) return a;
    if (comp(b->key, a->key)) a.swap(b);
    a->right = mergeNodes(std::move(a->right), std::move(b), comp);
    if (nplOf(a->left) < nplOf(a->right)) a->left.swap(a->right);
    a->npl = nplOf(a->right) + 1;
    return a;
  }

public:
  using value_type = T;

  LeftistHeap() = default;
  explicit LeftistHeap(Compare cmp) : comp(std::move(cmp)) {}

  template <std::ranges::input_range R>
  requires std::convertible_to<std::ranges::range_value_t<R>, T>
  explicit LeftistHeap(R&& r, Compare cmp = {}) : comp(std::move(cmp)) {
    for (auto&& x : r) push(static_cast<T>(std::forward<decltype(x)>(x)));
  }

  [[nodiscard]] bool empty() const noexcept { return count_ == 0; }
  [[nodiscard]] std::size_t size() const noexcept { return count_; }

  [[nodiscard]] const T& top() const {
    if (!root) throw std::runtime_error("LeftistHeap::top on empty heap");
    return root->key;
  }

  void push(const T& v) { emplace(v); }
  void push(T&& v) { emplace(std::move(v)); }

  template<class... Args>
  T& emplace(Args&&... args) {
    auto single = std::make_unique<Node>(T(std::forward<Args>(args)...));
    root = mergeNodes(std::move(root), std::move(single), comp);
    ++count_;
    return root->key;
  }

  void pop() {
    if (!root) throw std::runtime_error("LeftistHeap::pop on empty heap");
    auto l = std::move(root->left);
    auto r = std::move(root->right);
    root = mergeNodes(std::move(l), std::move(r), comp);
    --count_;
  }

  [[nodiscard]] T extractTop() {
    if (!root) throw std::runtime_error("LeftistHeap::extractTop on empty heap");
    T res = std::move(root->key);
    pop();
    return res;
  }

  void merge(LeftistHeap& other) {
    if (this == &other) return;
    root = mergeNodes(std::move(root), std::move(other.root), comp);
    count_ += std::exchange(other.count_, 0);
  }

  void merge(LeftistHeap&& other) { merge(other); }

  void clear() noexcept {
    root.reset();
    count_ = 0;
  }
};
