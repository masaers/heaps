#ifndef BINARY_HEAP_HPP
#define BINARY_HEAP_HPP
#include <functional>
#include <vector>

namespace com_masaers {

  template<typename Value,
	   typename Comp = std::less<Value>,
	   template<typename...> class Container = std::vector>
  class binary_heap {
  public:
    typedef std::size_t position_type;
    typedef typename std::decay<Value>::type value_type;
    typedef typename std::decay<Comp>::type comp_type;
  protected:
    struct node_t {
      template<typename CallValue>
      inline node_t(CallValue&& value, position_type position)
	: value_m(std::forward<CallValue>(value)), position_m(position)
      {}
      inline node_t(const node_t&) = default;
      inline node_t(node_t&&) = default;
      inline node_t& operator=(node_t x) {
	swap(*this, x);
	return *this;
      }
      friend inline void swap(node_t& a, node_t& b) {
	using namespace std;
	swap(a.value_m, b.value_m);
	swap(a.position_m, b.position_m);
      }
      value_type value_m;
      position_type position_m;
    }; // node_t
    struct comp_t {
      template<typename CallComp> inline comp_t(CallComp&& comp)
	: comp_m(std::forward<CallComp>(comp))
      {}
      inline comp_t(const comp_t&) = default;
      inline comp_t(comp_t&&) = default;
      inline comp_t& operator=(comp_t x) {
	swap(*this, x);
	return *this;
      }
      friend inline void swap(comp_t& a, comp_t& b) {
	using namespace std;
	swap(a.comp_m, b.comp_m);
      }
      inline bool operator()(const node_t& a, const node_t& b) {
	return comp_m(a.value_m, b.value_m);
      }
      comp_type comp_m;
    }; // comp_t
  public:
    typedef node_t* handle_type;
    typedef Container<handle_type> container_type;
    typedef typename container_type::const_iterator const_iterator;
    binary_heap() : container_m(), comp_m(comp_type()) {}
    binary_heap(const binary_heap&) = default;
    binary_heap(binary_heap&&) = default;
    template<typename CallValue>
    inline handle_type push(CallValue value) {
      handle_type result = new node_t(std::forward<CallValue>(value),
				       container_m.size());
      container_m.push_back(result);
      bubble_up(result);
      return result;
    }
    inline const value_type& top() const {
      return container_m.front()->value_m;
    }
    void pop() {
      swap_nodes(container_m.front(), container_m.back());
      delete container_m.back();
      container_m.pop_back();
      bubble_down(container_m.front());
    }
    bool empty() const { return container_m.empty(); }
    const_iterator begin() const { return container_m.begin(); }
    const_iterator end() const { return container_m.end(); }
    const_iterator cbegin() const { return container_m.begin(); }
    const_iterator cend() const { return container_m.end(); }
    /**
     * The new value needs to be *comparable to* `value_type`, that
     * is: the following expressions need to be valid:
     * `comp_type()(value_type(), CallValue())` and
     * `comp_type()(CallValue(), value_type())`.
     *
     * The new value needs to be *assignable to* `value_type`, that
     * is: the following expression needs to be valid: `value_type v =
     * CallValue()`.
     *
     * These two conditions allows the end-user to write custom
     * `value_type` structures that can hold the priority as well as
     * additional payload, and provide the comparator to the priority
     * as well as accepting only a priority value as parameter to the
     * assignment operator.
     */
    template<typename CallValue>
    void update(handle_type node, CallValue&& new_value) {
      if (comp_m.comp_m(new_value, node->value_m)) {
	node->value_m = new_value;
	bubble_up(node);
      } else if (comp_m.comp_m(node->value_m, new_value)) {
	node->value_m = new_value;
	bubble_down(node);
      } else {
	node->value_m = new_value;
      }
    }
    template<typename CallValue>
    bool ensure_priority(handle_type node, CallValue&& new_value) {
      bool result = false;
      if (comp_m.comp_m(new_value, node->value_m)) {
	node->value_m = new_value;
	bubble_up(node);
	result = true;
      }
      return result;
    }
    const value_type& value(handle_type node) const {
      return node->value_m;
    }
  protected:
    void bubble_up(handle_type node) {
      while (! root_b(node) && comp_m(*node, *parent(node))) {
	swap_nodes(node, parent(node));
      }
    }
    void bubble_down(handle_type node) {
      while (true) {
	handle_type lc = lchild(node);
	handle_type rc = rchild(node);
	if (lc == NULL) {
	  break;
	} else if (rc == NULL || comp_m(*lc, *rc)) {
	  if (comp_m(*lc, *node)) {
	    swap_nodes(node, lc);
	  } else {
	    break;
	  }
	} else {
	  if (comp_m(*rc, *node)) {
	    swap_nodes(node, rc);
	  } else {
	    break;
	  }
	}
      }
    }
    inline void swap_nodes(handle_type a, handle_type b) {
      using namespace std;
      swap(container_m[a->position_m], container_m[b->position_m]);
      swap(a->position_m, b->position_m);
    }
    // node:   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16  n
    // parent: -  0  0  1  1  2  2  3  3  4  4  5  5  6  6  7  7  (n-1)/2
    // lchild: 1  3  5  7  9 11 13 15                             (n*2)+1
    // rchild: 2  4  6  8 10 12 14 16                             (n*2)+2
    inline bool root_b(const handle_type node) const {
      return node->position_m == 0;
    }
    inline bool valid_b(const handle_type node) const {
      return node->position_m < container_m.size();
    }
    inline handle_type parent(const handle_type node) const {
      handle_type result = NULL;
      if (! root_b(node)) {
	result = container_m[(node->position_m - 1) / 2];
      }
      return result;
    }
    inline handle_type lchild(const handle_type node) const {
      handle_type result = NULL;
      const position_type position = (node->position_m * 2) + 1;
      if (position < container_m.size()) {
	result = container_m[position];
      }
      return result;
    }
    inline handle_type rchild(const handle_type node) const {
      handle_type result = NULL;
      const position_type position = (node->position_m * 2) + 2;
      if (position < container_m.size()) {
	result = container_m[position];
      }
      return result;
    }
    container_type container_m;
    comp_t comp_m;
  }; // binary_heap

  template<typename Value, typename Comp = std::less<Value> >
  binary_heap<Value, Comp, std::vector>
  make_binary_heap(const Comp& comp = Comp()) {
    return binary_heap<Value, Comp, std::vector>();
  }

} // namespace com_masaers

/******************************************************************************/
#endif
