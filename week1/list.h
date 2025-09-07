#include <bits/stdc++.h>

using i64 = long long;

template <class T> requires std::movable<T>   
class LinkedList {
  struct Node {
    T value;
    std::unique_ptr<Node> next;
    explicit Node(T v) : value(std::move(v)), next(nullptr) {}
  };

  std::unique_ptr<Node> head_;
  Node* tail_ = nullptr; 
  std::size_t size_ = 0;

	public:
    LinkedList() = default;
    LinkedList(const LinkedList&) = delete;            
    LinkedList& operator=(const LinkedList&) = delete;
    LinkedList(LinkedList&&) noexcept = default;
    LinkedList& operator=(LinkedList&&) noexcept = default;

    ~LinkedList() { clear(); }

    bool empty() const noexcept { return size_ == 0; }
    std::size_t size() const noexcept { return size_; }

    T& front() {
      if (empty()) throw std::runtime_error("front() on empty list");
      return head_->value;
    }
    const T& front() const {
      if (empty()) throw std::runtime_error("front() on empty list");
      return head_->value;
    }
    T& back() {
      if (empty()) throw std::runtime_error("back() on empty list");
      return tail_->value;
    }
    const T& back() const {
      if (empty()) throw std::runtime_error("back() on empty list");
      return tail_->value;
    }
    
    void push_front(T value) {
      auto node = std::make_unique<Node>(std::move(value));
      node->next = std::move(head_);
      head_ = std::move(node);
      if (!tail_) tail_ = head_.get();
      ++size_;
    }

    void push_back(T value) {
      auto node = std::make_unique<Node>(std::move(value));
      Node* raw = node.get();
      if (tail_) {
        tail_->next = std::move(node);
        tail_ = raw;
      } else {
        head_ = std::move(node);
        tail_ = head_.get();
      }
      ++size_;
    }
    
    bool pop_front(T& out) {
      if (empty()) return false;
      out = std::move(head_->value);
      auto old = std::move(head_);
      head_ = std::move(old->next);
      if (!head_) tail_ = nullptr;
      --size_;
      return true;
    }

  void clear() noexcept {
    while (head_) {
      auto next = std::move(head_->next);
      head_.reset();
      head_ = std::move(next);
    }
    tail_ = nullptr;
    size_ = 0;
  }
};

template <class T> requires std::movable<T>
class LinkedQueue {
    LinkedList<T> list_;
	public:
  	LinkedQueue() = default;

  template <std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, T>
  explicit LinkedQueue(R&& r) {
    for (auto&& x : r) enqueue(static_cast<T>(x));
  }

  bool empty() const noexcept { return list_.empty(); }
  std::size_t size() const noexcept { return list_.size(); }

  void enqueue(T value) { list_.push_back(std::move(value)); }
  bool dequeue(T& out)  { return list_.pop_front(out); }

  T& front() { return list_.front(); }
  const T& front() const { return list_.front(); }
  T& back() { return list_.back(); }
  const T& back() const { return list_.back(); }

  void clear() noexcept { list_.clear(); }
};

template <class T>
class LinkedStack {
  struct Node { 
    T val; 
    Node* next; 
    Node(const T& v, Node* n) : val(v), next(n) {} 
  };
  Node* head_ = nullptr; 
  size_t sz_ = 0;

  public:
    ~LinkedStack() { clear(); }
    bool empty() const { return sz_ == 0; }
    size_t size() const { return sz_; }

    void push(const T& v) { head_ = new Node(v, head_); ++sz_; }
    bool pop(T& out) {
      if(empty()) return false;
      Node* p=head_; 
      out=p->val;
      head_=p->next; 
      delete p; 
      --sz_; 
      
      return true;
    }
    
    T& top() { return head_->val; }
    T& second() { return head_->next->val; }
    void clear() { 
      while(head_) { 
        Node* n=head_->next; 
        delete head_; 
        head_=n; 
      }

      size_=0; 
    }
};

