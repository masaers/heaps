#ifndef MUTABLE_HEAP_HPP
#define MUTABLE_HEAP_HPP
// c++
#include <functional>
#include <vector>
// c


namespace com_masaers {
  
  template<typename value_T,
	   typename comp_T = std::less<value_T>,
	   template<typename...> class container_T = std::vector>
  class mutable_min_heap {
  public:
    typedef std::size_t position_type;
    typedef typename std::decay<value_T>::type value_type;
    typedef typename std::decay<comp_T>::type comp_type;
  protected:
    struct node_t;
    template<typename handled_T> struct handle_t;
  public:
    typedef handle_t<node_t> handle_type;
    typedef handle_t<const node_t> const_handle_type;
    typedef container_T<handle_type> container_type;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;
    
    mutable_min_heap(const comp_T& comp = comp_T())
      : container_m(), comp_m(comp)
    {}
    mutable_min_heap(const mutable_min_heap& x)
      : container_m(x.container_m), comp_m(x.comp_m)
    {
      for (auto it = container_m.begin(); it != container_m.end(); ++it) {
	it->node_m = new node_t(*it->node_m);
      }
    }
    mutable_min_heap(mutable_min_heap&&) = default;
    ~mutable_min_heap() { clear(); }
    mutable_min_heap& operator=(mutable_min_heap x) {
      swap(*this, x);
      return *this;
    }
    friend void swap(mutable_min_heap& a, mutable_min_heap& b) {
      using std::swap;
      swap(a.container_m, b.container_m);
      swap(a.comp_m, b.comp_m);
    }
    template<typename T> handle_type push(T&& value) {
      handle_type result(new node_t(std::forward<T>(value),
                                    container_m.size()));
      container_m.push_back(result);
      bubble_up(result);
      return result;
    }
    const value_type& top() const {
      return container_m.front().value();
    }
    void pop() {
      swap_handles(container_m.front(), container_m.back());
      delete container_m.back().node_m;
      container_m.pop_back();
      if (! container_m.empty()) {
	bubble_down(container_m.front());
      }
    }
    value_type pop(value_type&& value) {
      using std::swap;
      swap(value, container_m.back().value());
      pop();
      return value;
    }
    value_type& pop(value_type& value) {
      using std::swap;
      swap(value, container_m.back().value());
      pop();
      return value;
    }
    ///
    /// Swaps the top and a supplied element, maintaining the heap
    /// property. Note that the element being swapped in might end up
    /// anywhere in the heap.
    ///
    void swap_top(value_type& other) {
      using std::swap;
      swap(container_m.front().value(), other);
      bubble_down(container_m.front());
    }
    ///
    /// Replaces the top element of the heap with one constructed out
    /// of the supplied arguments. Note that the new element might end
    /// up anywhere in the heap.
    ///
    template<typename... args_T> void emplace_top(args_T&&... args) {
      container_m.front().value() = value_type(std::forward<args_T>(args)...);
      bubble_down(container_m.front());
    }
    void erase(handle_type handle) {
      handle_type replacement(container_m.back());
      swap_nodes(handle, replacement);
      container_m.back().clear();
      container_m.pop_back();
      if (! container_m.empty()) {
	maintain_update(replacement);
      }
    }
    bool empty() const { return container_m.empty(); }
    std::size_t size() const { return container_m.size(); }
    void clear() {
      for (auto it = container_m.begin(); it != container_m.end(); ++it) {
	it->clear();
      }
      container_m.clear();
    }
    ///
    /// This functions does not work, since the heap property can be
    /// fulfilled in multiple ways...
    ///
    bool operator==(const mutable_min_heap& x) const {
      bool result = container_m.size() == x.container_m.size();
      if (result) {
	for (auto it = container_m.begin(), jt = x.container_m.begin();
	     result && it != container_m.end();
	     ++it, ++jt) {
	  result = it->value() == jt->value;
	}
      }
      return result;
    }
    bool operator!=(const mutable_min_heap& x) const { return ! operator==(x); }
    const_iterator cbegin() const { return container_m.begin(); }
    const_iterator cend() const { return container_m.end(); }
    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }
    iterator begin() { return container_m.begin(); }
    iterator end() { return container_m.end(); }
    bool maintain_towards_top(handle_type handle) {
      return bubble_up(handle);
    }
    bool maintain_towards_bottom(handle_type handle) {
      return bubble_down(handle);
    }
    bool maintain_update(handle_type handle) {
      return bubble_up(handle) || bubble_down(handle);
    }
  protected:
    bool bubble_up(handle_type handle) {
      bool result = false;
      while (! root_b(handle) &&
             comp_m(handle.value(), parent(handle).value())) {
	swap_handles(handle, parent(handle));
	result = true;
      }
      return result;
    }
    bool bubble_down(handle_type handle) {
      bool result = false;
      while (true) {
	handle_type lc = lchild(handle);
	handle_type rc = rchild(handle);
	if (lc == handle_type()) {
	  // Tree left heavy, no left child means no right child
	  // No child to swap with
	  break;
	} else if (rc == handle_type() || comp_m(lc.value(), rc.value())) {
	  // The left child is the smallest child
	  if (comp_m(lc.value(), handle.value())) {
	    swap_handles(handle, lc);
	    result = true;
	  } else {
	    // Swapping with left child would break heap property
	    break;
	  }
	} else {
	  // Right child exists and is less than the left child
	  if (comp_m(rc.value(), handle.value())) {
	    swap_handles(handle, rc);
	    result = true;
	  } else {
	    // Node is less than both children
	    break;
	  }
	}
      }
      return result;
    }
    void swap_handles(handle_type a, handle_type b) {
      using std::swap;
      swap(container_m[a.position()], container_m[b.position()]);
      swap(a.position(), b.position());
    }
    // node:   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16  n
    // parent: -  0  0  1  1  2  2  3  3  4  4  5  5  6  6  7  7  (n-1)/2
    // lchild: 1  3  5  7  9 11 13 15                             (n*2)+1
    // rchild: 2  4  6  8 10 12 14 16                             (n*2)+2
    bool root_b(const_handle_type handle) const {
      return handle.position() == 0;
    }
    bool valid_b(const_handle_type handle) const {
      return handle.position() < container_m.size();
    }
    handle_type parent(const_handle_type handle) const {
      handle_type result;
      if (! root_b(handle)) {
	result = container_m[(handle.position() - 1) / 2];
      }
      return result;
    }
    handle_type lchild(const_handle_type handle) const {
      handle_type result;
      const position_type position = (handle.position() * 2) + 1;
      if (position < container_m.size()) {
	result = container_m[position];
      }
      return result;
    }
    handle_type rchild(const_handle_type handle) const {
      handle_type result;
      const position_type position = (handle.position() * 2) + 2;
      if (position < container_m.size()) {
	result = container_m[position];
      }
      return result;
    }
    container_type container_m;
    comp_type comp_m;
  }; // mutable_min_heap

  
  template<typename value_T,
	   typename comp_T,
	   template<typename...> class container_T>
  struct mutable_min_heap<value_T, comp_T, container_T>::node_t {
    template<typename T>
    node_t(T&& value, position_type position)
      : value_m(std::forward<T>(value)), position_m(position)
    {}
    node_t(const node_t&) = default;
    node_t(node_t&&) = default;
    node_t& operator=(node_t x) {
      swap(*this, x);
      return *this;
    }
    friend void swap(node_t& a, node_t& b) {
      using std::swap;
      swap(a.value_m, b.value_m);
      swap(a.position_m, b.position_m);
    }
    value_type value_m;
    position_type position_m;
  }; // node_t

  
  template<typename value_T,
	   typename comp_T,
	   template<typename...> class container_T>
  template<typename handled_T>
  struct mutable_min_heap<value_T, comp_T, container_T>::handle_t {
    friend class mutable_min_heap;
    typedef typename std::conditional<std::is_const<handled_T>::value, const value_type, value_type>::type handled_value_type;
    typedef typename std::conditional<std::is_const<handled_T>::value, const position_type, position_type>::type handled_position_type;
    handle_t() : node_m(NULL) {}
    template<typename U> handle_t(const handle_t<U>& x) : node_m(x.node_m) {}
    template<typename U> handle_t(U* x) : node_m(x) {}
    handle_t(const handle_t&) = default;
    handle_t(handle_t&&) = default;
    handle_t& operator=(handle_t x) {
      swap(*this, x);
      return *this;
    }
    friend void swap(handle_t& a, handle_t& b) {
      using std::swap;
      swap(a.node_m, b.node_m);
    }
    handled_value_type& operator*() const { return node_m->value_m; }
    handled_value_type* operator->() const { return &node_m->value_m; }
    template<typename T>
    bool operator==(T* const x) const { return node_m == x; }
    template<typename T>
    bool operator==(const handle_t<T> x) const { return node_m == x.node_m; }
    template<typename T>
    bool operator!=(T&& x) const { return ! operator==(std::forward<T>(x)); }
    handled_value_type& value() { return node_m->value_m; }
    const handled_value_type& value() const { return node_m->value_m; }
    handled_position_type& position() { return node_m->position_m; }
    const handled_position_type& position() const { return node_m->position_m; }
    void clear() { delete node_m; node_m = NULL; }
  protected:
    handled_T* node_m;
  }; // handle_t


  template<typename value_T,
	   template<typename...> class container_T = std::vector,
	   typename comp_T = std::less<value_T> >
  mutable_min_heap<value_T, typename std::decay<comp_T>::type, container_T>
  make_mutable_min_heap(comp_T&& comp = comp_T()) {
    return mutable_min_heap<value_T, typename std::decay<comp_T>::type, container_T>(std::forward<comp_T>(comp));
  }

  // template<typename value_T,
  //          template<typename...> class container_T = std::vector,
  //          typename comp_T = std::less<value_T> >
  // mutable_min_heap<value_T, reverse_comp<typename std::decay<comp_T>::type>, container_T>
  // make_mutable_max_heap(comp_T&& comp = comp_T()) {
  //   return mutable_min_heap<value_T, reverse_comp<typename std::decay<comp_T>::type>, container_T>(std::forward<comp_T>(comp));
  // }
} // namespace util


/******************************************************************************/
#endif
