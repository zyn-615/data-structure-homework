#pragma once
#include <vector>
#include <concepts>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <optional>
#include <ranges>

template<class T, class Compare = std::less<>>
requires std::strict_weak_order<Compare, T, T>
class Heap {
  std::vector<T> data;
  Compare comp;

  static constexpr std::size_t parent_index(std::size_t i) noexcept { return (i - 1) >> 1; }
  static constexpr std::size_t left_child(std::size_t i)   noexcept { return (i << 1) + 1; }
  static constexpr std::size_t right_child(std::size_t i)  noexcept { return (i << 1) + 2; }

  void sift_up(std::size_t i) {
    while (i && comp(data[parent_index(i)], data[i])) {
      std::swap(data[parent_index(i)], data[i]);
      i = parent_index(i);
    }
  }

  void sift_down(std::size_t i) {
    for (;;) {
      std::size_t l = left_child(i), r = right_child(i), bestIdx = i;
      if (l < data.size() && comp(data[bestIdx], data[l])) bestIdx = l;
      if (r < data.size() && comp(data[bestIdx], data[r])) bestIdx = r;
      if (bestIdx == i) break;
      std::swap(data[i], data[bestIdx]);
      i = bestIdx;
    }
  }
  
  void heapify() {
    if (data.empty()) return;
    for (std::size_t i = data.size() / 2; i-- > 0; ) sift_down(i);
  }

public:
  using value_type = T;
  using size_type = std::size_t;
  using reference = T&;
  using const_reference = const T&;

  Heap() = default;
  explicit Heap(Compare c) : comp(std::move(c)) {}
  Heap(std::initializer_list<T> init, Compare c = {}) : data(init), comp(std::move(c)) { heapify(); }

  template<std::ranges::input_range R>
  requires std::convertible_to<std::ranges::range_value_t<R>, T>
  explicit Heap(R&& r, Compare c = {}) : comp(std::move(c)) {
    if constexpr (std::ranges::sized_range<R>) data.reserve(std::ranges::size(r));
    for (auto&& x : r) data.emplace_back(std::forward<decltype(x)>(x));
    heapify();
  }

  [[nodiscard]] bool empty() const noexcept { return data.empty(); }
  [[nodiscard]] size_type size() const noexcept { return data.size(); }

  [[nodiscard]] const_reference top() const {
    if (data.empty()) throw std::out_of_range("Heap::top on empty heap");
    return data.front();
  }

  void push(const T& x) {
    data.push_back(x);
    sift_up(data.size() - 1);
  }
  void push(T&& x) {
    data.push_back(std::move(x));
    sift_up(data.size() - 1);
  }
  template<class... Args>
  reference emplace(Args&&... args) {
    data.emplace_back(std::forward<Args>(args)...);
    sift_up(data.size() - 1);
    return data.back();
  }

  [[nodiscard]] T pop() {
    if (data.empty()) throw std::out_of_range("Heap::pop on empty heap");
    std::swap(data.front(), data.back());
    T ret = std::move(data.back());
    data.pop_back();
    if (!data.empty()) sift_down(0);
    return ret;
  }

  [[nodiscard]] std::optional<T> tryPop() {
    if (data.empty()) return std::nullopt;
    return pop();
  }
};
