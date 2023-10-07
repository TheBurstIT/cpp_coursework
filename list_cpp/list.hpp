#pragma once
#include <cstddef>
#include <iostream>

template <typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  struct BaseNode {
    BaseNode* next = this;
    BaseNode* prev = this;
  };

  struct Node : BaseNode {
    T value;
    Node() = default;
    explicit Node(const T& value) : value(value) {}
  };
  size_t size_;
  mutable BaseNode begin_;

  using node_alloc =
      typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
  using alloc_traits = typename std::allocator_traits<node_alloc>;
  void buildup(const T& value) const {
    BaseNode* prev_node = &begin_;
    size_t ind = 0;
    while (ind < size_) {
      Node* current = alloc_traits::allocate(alloc_, 1);
      alloc_traits::construct(alloc_, current, value);
      current->next = &begin_;
      prev_node->next = current;
      current->prev = prev_node;
      if (ind + 1 == size_) {
        begin_.prev = current;
      }
      prev_node = current;
      ++ind;
    }
  }
  mutable node_alloc alloc_;

 public:
  using value_type = T;
  using allocator_type =
      typename std::allocator_traits<Allocator>::allocator_type;
  template <bool IsConst>
  class Iterator {
   public:
    using value_type = typename std::conditional_t<IsConst, const T, T>;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;
    pointer operator->() { return &(reinterpret_cast<Node*>(ptr_)->value); }

    bool operator==(Iterator other) { return ptr_ == other.ptr_; }

    bool operator!=(Iterator other) { return !(*this == other); }  //!=(???)

    reference operator*() { return reinterpret_cast<Node*>(ptr_)->value; }
    Iterator& operator++() {
      ptr_ = ptr_->next;
      return *this;
    }
    Iterator operator++(int) {
      Iterator res = *this;
      ++(*this);
      return res;
    }
    Iterator& operator--() {
      ptr_ = ptr_->prev;
      return *this;
    }
    Iterator operator--(int) {
      Iterator res = *this;
      --(*this);
      return res;
    }
    Iterator& operator=(const Iterator& other) {
      ptr_ = other.ptr_;
      return *this;
    }
    operator List<T, Allocator>::Iterator<true>() {
      return List<T, Allocator>::Iterator<true>(ptr_);
    }
    Iterator(BaseNode* ptr) : ptr_(ptr) {}
    Iterator(const Iterator& other) : ptr_(other.ptr_) {}

    BaseNode* get_ptr() { return ptr_; }

   private:
    BaseNode* ptr_;
  };

  using iterator = Iterator<false>;
  using reverse_iterator = std::reverse_iterator<Iterator<false>>;
  using const_iterator = Iterator<true>;
  using const_reverse_iterator = std::reverse_iterator<Iterator<true>>;

  List(size_t size) : size_(size) {
    BaseNode* prev_node = &begin_;
    size_t ind = 0;
    Node* current;
    while (ind < size_) {
      try {
        current = alloc_traits::allocate(alloc_, 1);
      } catch (...) {
        while (ind > 0) {
          current = reinterpret_cast<Node*>(prev_node);
          prev_node = prev_node->prev;
          alloc_traits::destroy(alloc_, current);
          alloc_traits::deallocate(alloc_, current, 1);
          --ind;
        }
        throw;
      }
      try {
        alloc_traits::construct(alloc_, current);
      } catch (...) {
        alloc_traits::deallocate(alloc_, current, 1);
        while (ind > 0) {
          current = reinterpret_cast<Node*>(prev_node);
          prev_node = prev_node->prev;
          alloc_traits::destroy(alloc_, current);
          alloc_traits::deallocate(alloc_, current, 1);
          --ind;
        }
        throw;
      }
      current->next = &begin_;
      prev_node->next = current;
      current->prev = prev_node;
      if (ind + 1 == size_) {
        begin_.prev = current;
      }
      prev_node = current;
      ++ind;
    }
  }
  List(size_t size, const T& value, const Allocator& alloc)
      : size_(size), alloc_(alloc) {
    buildup(value);
  }
  List(const Allocator& alloc) : size_(0), alloc_(alloc) {}
  List() : size_(0) {}
  List(size_t size, const T& value) : size_(size) { buildup(value); }
  List(size_t size, const Allocator& alloc) : size_(size), alloc_(alloc) {
    size_t ind = 0;
    BaseNode* prev_node = &begin_;
    Node* current;
    while (ind < size_) {
      try {
        current = alloc_traits::allocate(alloc_, 1);
      } catch (...) {
        while (ind > 0) {
          current = reinterpret_cast<Node*>(prev_node);
          prev_node = prev_node->prev;
          alloc_traits::destroy(alloc_, current);
          alloc_traits::deallocate(alloc_, current, 1);
          ind--;
        }
        throw;
      }
      try {
        alloc_traits::construct(alloc_, current);
      } catch (...) {
        alloc_traits::deallocate(alloc_, current, 1);
        while (ind > 0) {
          current = reinterpret_cast<Node*>(prev_node);
          prev_node = prev_node->prev;
          alloc_traits::destroy(alloc_, current);
          alloc_traits::deallocate(alloc_, current, 1);
          ind--;
        }
        throw;
      }
      current->prev = prev_node;
      prev_node->next = current;
      current->next = &begin_;
      if (size_ == ind + 1) {
        begin_.prev = current;
      }
      prev_node = current;
      ++ind;
    }
  }
  List(const List& other) {
    alloc_ = alloc_traits::select_on_container_copy_construction(other.alloc_);
    const BaseNode* other_prev = &other.begin_;
    size_t ind = 0;
    while (ind < other.size_) {
      BaseNode* other_current = other_prev->next;
      try {
        push_back(reinterpret_cast<Node*>(other_current)->value);
      } catch (...) {
        for (; ind > 0; --ind) {
          pop_back();
        }
        throw;
      }
      other_prev = other_current;
      ind++;
    }
    size_ = other.size_;
  }
  List(std::initializer_list<T> initializer_list,
       const Allocator& allocator = Allocator())
      : alloc_(allocator) {
    size_ = 0;
    for (const auto& elem : initializer_list) {
      push_back(elem);
    }
  }
  ~List() {
    while (size_ > 0) {
      pop_back();
    }
  }
  List& operator=(const List& other) {
    if (this != &other) {
      size_t initial_size = size_;
      if (alloc_traits::propagate_on_container_copy_assignment::value) {
        alloc_ = other.alloc_;
      }
      size_t ind = 0;
      const BaseNode* other_prev = &other.begin_;
      while (ind < other.size_) {
        BaseNode* other_current = other_prev->next;
        try {
          push_back(reinterpret_cast<Node*>(other_current)->value);
        } catch (...) {
          for (; ind > 0; --ind) {
            pop_back();
          }
          throw;
        }
        other_prev = other_current;
        ++ind;
      }
      for (; initial_size > 0; --initial_size) {
        pop_front();
      }
      size_ = other.size_;
    }
    return *this;
  }
  size_t size() const { return size_; }

  Allocator get_allocator() const { return alloc_; }

  void push_back(const T& value) {
    Node* new_node;
    try {
      new_node = alloc_traits::allocate(alloc_, 1);
    } catch (...) {
      throw;
    }
    try {
      alloc_traits::construct(alloc_, new_node, value);
    } catch (...) {
      alloc_traits::deallocate(alloc_, new_node, 1);
      throw;
    }
    ++size_;
    BaseNode* last_node = begin_.prev;
    new_node->next = &begin_;
    begin_.prev = new_node;
    last_node->next = new_node;
    new_node->prev = last_node;
  }

  void push_front(const T& value) {
    Node* new_node;
    try {
      new_node = alloc_traits::allocate(alloc_, 1);
    } catch (...) {
      throw;
    }
    try {
      alloc_traits::construct(alloc_, new_node, value);
    } catch (...) {
      alloc_traits::deallocate(alloc_, new_node, 1);
    }
    ++size_;
    BaseNode* first_node = begin_.next;
    new_node->prev = &begin_;
    begin_.next = new_node;
    first_node->prev = new_node;
    new_node->next = first_node;
  }

  void pop_front() {
    BaseNode* tmp = begin_.next->next;
    alloc_traits::destroy(alloc_, reinterpret_cast<Node*>(begin_.next));
    alloc_traits::deallocate(alloc_, reinterpret_cast<Node*>(begin_.next), 1);
    tmp->prev = &begin_;
    --size_;
    begin_.next = tmp;
  }

  void pop_back() {
    BaseNode* tmp = begin_.prev->prev;
    alloc_traits::destroy(alloc_, reinterpret_cast<Node*>(begin_.prev));
    alloc_traits::deallocate(alloc_, reinterpret_cast<Node*>(begin_.prev), 1);
    tmp->next = &begin_;
    begin_.prev = tmp;
    --size_;
  }

  void erase(const_iterator iter) {
    BaseNode* tmp_prev = iter.get_ptr()->prev;
    BaseNode* tmp_next = iter.get_ptr()->next;
    alloc_traits::destroy(alloc_, reinterpret_cast<Node*>(iter.get_ptr()));
    alloc_traits::deallocate(alloc_, reinterpret_cast<Node*>(iter.get_ptr()),
                             1);
    --size_;
    tmp_next->prev = tmp_prev;
    tmp_prev->next = tmp_next;
  }

  void insert(const_iterator iter, const T& value) {
    Node* new_node = alloc_traits::allocate(alloc_, 1);
    alloc_traits::construct(alloc_, new_node, value);
    new_node->prev = iter.get_ptr()->prev;
    iter.get_ptr()->prev->next = new_node;
    iter.get_ptr()->prev = new_node;
    ++size_;
    new_node->next = iter.get_ptr();
  }

  bool empty() const { return size_ == 0; }

  iterator begin() { return iterator(begin_.next); }

  iterator end() { return iterator(&begin_); }

  const_iterator begin() const { return cbegin(); }

  const_iterator end() const { return cend(); }

  const_iterator cbegin() const { return const_iterator(begin_.next); }

  const_iterator cend() const { return const_iterator(&begin_); }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(begin_.prev);
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(&begin_);
  }

  reverse_iterator rbegin() { return reverse_iterator(begin_.prev); }

  reverse_iterator rend() { return reverse_iterator(&begin_); }

  const_reverse_iterator rbegin() const { return crbegin(); }

  reverse_iterator rend() const { return crend(); }
};
