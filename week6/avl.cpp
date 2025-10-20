#include <algorithm>
#include <cassert>
#include <concepts>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
#include <utility>
#include <vector>

template <typename Comp, typename Key>
concept KeyComparator = std::strict_weak_order<Comp, Key, Key>;

template <typename Key, typename T, KeyComparator<Key> Compare = std::less<Key>>
class avl_tree {
  private:
    struct Node {
      std::pair<Key, T> kv;
      std::unique_ptr<Node> left{};
      std::unique_ptr<Node> right{};
      int height = 1;

      explicit Node(std::pair<Key, T>&& p) : kv(std::move(p)) {}
      explicit Node(const Key& k, const T& v) : kv(k, v) {}
      template <typename... Args>
      Node(const Key& k, std::in_place_t, Args&&... args) : kv(k, T(std::forward<Args>(args)...)) {}
    };

    std::unique_ptr<Node> root_{};
    std::size_t size_ = 0;
    [[no_unique_address]] Compare comp_{};

    static int height_of(const std::unique_ptr<Node>& n) noexcept { return n ? n->height : 0; }
    static int bf_of(const std::unique_ptr<Node>& n) noexcept { return n ? height_of(n->left) - height_of(n->right) : 0; }
    static void update(Node* n) noexcept {
      n->height = 1 + std::max(height_of(n->left), height_of(n->right));
    }

    static void rotate_right(std::unique_ptr<Node>& y) noexcept {
      auto x = std::move(y->left);
      auto T2 = std::move(x->right);
      x->right = std::move(y);
      x->right->left = std::move(T2);
      update(x->right.get());
      update(x.get());
      y = std::move(x);
    }

    static void rotate_left(std::unique_ptr<Node>& x) noexcept {
      auto y = std::move(x->right);
      auto T2 = std::move(y->left);
      y->left = std::move(x);
      y->left->right = std::move(T2);
      update(y->left.get());
      update(y.get());
      x = std::move(y);
    }

    static void rebalance(std::unique_ptr<Node>& n) noexcept {
      update(n.get());
      const int bf = bf_of(n);
      if (bf > 1) {
        if (bf_of(n->left) < 0) rotate_left(n->left);
        rotate_right(n);
      } else if (bf < -1) {
        if (bf_of(n->right) > 0) rotate_right(n->right);
        rotate_left(n);
      }
    }

    Node* find_node(const Key& key) noexcept {
      auto* cur = root_.get();
      while (cur) {
        if (comp_(key, cur->kv.first)) cur = cur->left.get();
        else if (comp_(cur->kv.first, key)) cur = cur->right.get();
        else return cur; 
      }
      return nullptr;
    }

    const Node* find_node(const Key& key) const noexcept {
      auto* cur = root_.get();
      while (cur) {
        if (comp_(key, cur->kv.first)) cur = cur->left.get();
        else if (comp_(cur->kv.first, key)) cur = cur->right.get();
        else return cur;
      }
      return nullptr;
    }

    std::pair<T&, bool> insert_or_assign_impl(std::unique_ptr<Node>& n, const Key& key, T&& value) {
      if (!n) { n = std::make_unique<Node>(std::pair<Key, T>(key, std::move(value))); ++size_; return {n->kv.second, true}; }
      if (comp_(key, n->kv.first)) {
        auto [ref, ins] = insert_or_assign_impl(n->left, key, std::move(value));
        rebalance(n);
        return {ref, ins};
      }
      if (comp_(n->kv.first, key)) {
        auto [ref, ins] = insert_or_assign_impl(n->right, key, std::move(value));
        rebalance(n);
        return {ref, ins};
      }
      
      n->kv.second = std::move(value);
      return {n->kv.second, false};
    }

    template <typename... Args>
    std::pair<T&, bool> emplace_impl(std::unique_ptr<Node>& n, const Key& key, Args&&... args) {
      if (!n) {
        n = std::make_unique<Node>(key, std::in_place, std::forward<Args>(args)...);
        ++size_;
        return {n->kv.second, true};
      }
      if (comp_(key, n->kv.first)) {
        auto [ref, ins] = emplace_impl(n->left, key, std::forward<Args>(args)...);
        rebalance(n);
        return {ref, ins};
      }
      if (comp_(n->kv.first, key)) {
        auto [ref, ins] = emplace_impl(n->right, key, std::forward<Args>(args)...);
        rebalance(n);
        return {ref, ins};
      }
      
      return {n->kv.second, false};
    }

    static std::unique_ptr<Node> extract_min(std::unique_ptr<Node>& n) noexcept {
      if (!n->left) {
        auto min = std::move(n);
        n = std::move(min->right);
        return min;
      }
      auto min = extract_min(n->left);
      rebalance(n);
      return min;
    }

    bool erase_impl(std::unique_ptr<Node>& n, const Key& key) {
      if (!n) return false;
      if (comp_(key, n->kv.first)) {
        const bool ok = erase_impl(n->left, key);
        if (ok) rebalance(n);
        return ok;
      }
      if (comp_(n->kv.first, key)) {
        const bool ok = erase_impl(n->right, key);
        if (ok) rebalance(n);
        return ok;
      }

      --size_;
      if (!n->left) { n = std::move(n->right); return true; }
      if (!n->right) { n = std::move(n->left); return true; }

      auto left_sub = std::move(n->left);
      auto right_sub = std::move(n->right);
      auto min = extract_min(right_sub);
      min->left = std::move(left_sub);
      min->right = std::move(right_sub);
      update(min.get());
      n = std::move(min);
      rebalance(n);
      return true;
    }

    template <typename F>
    static void inorder_node(Node* n, F&& f) {
      if (!n) return;
      inorder_node(n->left.get(), f);
      std::invoke(f, n->kv.first, n->kv.second);
      inorder_node(n->right.get(), f);
    }

  public:
    avl_tree() = default;
    explicit avl_tree(Compare comp) : comp_(std::move(comp)) {}

    avl_tree(std::initializer_list<std::pair<Key, T>> init, Compare comp = {}) : comp_(std::move(comp)) {
      for (auto&& p : init) insert_or_assign(std::move(p.first), std::move(p.second));
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t { return size_; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }

    auto insert_or_assign(const Key& key, T value) -> std::pair<T&, bool> {
        auto res = insert_or_assign_impl(root_, key, std::move(value));
        rebalance(root_);
        return res;
    }

    template <typename... Args>
    requires std::constructible_from<T, Args...>
    auto emplace(const Key& key, Args&&... args) -> std::pair<T&, bool> {
      auto res = emplace_impl(root_, key, std::forward<Args>(args)...);
      rebalance(root_);
      return res;
    }

    [[nodiscard]] T* find(const Key& key) noexcept { if (auto* n = find_node(key)) return &n->kv.second; return nullptr; }
    [[nodiscard]] const T* find(const Key& key) const noexcept { if (auto* n = find_node(key)) return &n->kv.second; return nullptr; }

    [[nodiscard]] bool contains(const Key& key) const noexcept { return find_node(key) != nullptr; }

    bool erase(const Key& key) {
      const bool ok = erase_impl(root_, key);
      if (root_) rebalance(root_);
      return ok;
    }

    template <typename F>
    requires std::invocable<F&, const Key&, T&>
    void for_each_inorder(F f) {
      inorder_node(root_.get(), f);
    }
    template <typename F>
    requires std::invocable<F&, const Key&, const T&>
    void for_each_inorder(F f) const {
      inorder_node(root_.get(), f);
    }

    [[nodiscard]] auto to_vector() const -> std::vector<std::pair<Key, T>> {
      std::vector<std::pair<Key, T>> out;
      out.reserve(size_);
      for_each_inorder([&](const Key& k, const T& v){ out.emplace_back(k, v); });
      return out;
    }
};