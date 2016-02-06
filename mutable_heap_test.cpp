#include "mutable_heap.hpp"
#include <iostream>

#define _TEST_OUTPUT_PREFIX(stream)		\
  stream << __FILE__ << ":" << __LINE__ << " "; \
  
#define TEST(expr)                                                      \
  try {									\
    if (expr) {                                                         \
      _TEST_OUTPUT_PREFIX(std::cout);                                   \
      std::cout << #expr << " [PASSED]" << std::endl;                   \
    } else {								\
      _TEST_OUTPUT_PREFIX(std::cerr);					\
      std::cerr << #expr << " [FAILED]" << std::endl;			\
    }									\
  } catch (const std::exception& e) {					\
    _TEST_OUTPUT_PREFIX(std::cerr);                                     \
    std::cerr << #expr;							\
      std::cerr << " exception: \"" << e.what() << "\"";                \
      std::cerr << " [FAILED]" << std::endl;                            \
  } catch (...) {							\
    _TEST_OUTPUT_PREFIX(std::cerr);                                     \
    std::cerr << #expr << " unknown exception [FAILED]" << std::endl;	\
  }									\
  
#define TEST_INFO(...)						       \
  _TEST_OUTPUT_PREFIX(std::cout);				       \
  std::cout << #__VA_ARGS__ << " [EXECUTING]" << std::endl;	       \
  __VA_ARGS__;							       \
  

template<typename heap_T>
void test_max_heap(heap_T&& h, const char* name) {
  using namespace std;
  
  TEST_INFO(h.clear());
  TEST(h.empty());
  TEST_INFO(h.push(1));
  TEST_INFO(h.push(2));
  TEST_INFO(h.push(10));
  TEST_INFO(h.push(5));
  TEST(h.size() == 4);
  TEST(h.top() == 10);
  TEST_INFO(typename std::decay<heap_T>::type::value_type other = 12);
  TEST_INFO(h.swap_top(other));
  TEST(h.size() == 4);
  TEST(h.top() == 12);
  TEST(other == 10);
  TEST_INFO(h.emplace_top(other));
  TEST(h.size() == 4);
  TEST(h.top() == 10);
  TEST_INFO(h.pop());
  TEST(h.size() == 3);
  TEST(h.top() == 5);
  TEST_INFO(h.pop());
  TEST(h.size() == 2);
  TEST(h.top() == 2);
  TEST_INFO(h.pop());
  TEST(h.size() == 1);
  TEST(h.top() == 1);
  TEST_INFO(h.pop());
  TEST(h.size() == 0);
}

template<typename heap_T>
void test_min_heap(heap_T&& h, const char* name) {
  using namespace std;

  TEST_INFO(h.clear());
  TEST(h.empty());
  TEST_INFO(h.push(1));
  TEST_INFO(h.push(2));
  TEST_INFO(h.push(10));
  TEST_INFO(h.push(5));
  TEST(h.size() == 4);
  TEST(h.top() == 1);
  TEST_INFO(typename std::decay<heap_T>::type::value_type other = 0);
  TEST_INFO(h.swap_top(other));
  TEST(h.size() == 4);
  TEST(h.top() == 0);
  TEST(other == 1);
  TEST_INFO(h.emplace_top(other));
  TEST(h.size() == 4);
  TEST(h.top() == 1);
  TEST_INFO(h.pop());
  TEST(h.size() == 3);
  TEST(h.top() == 2);
  TEST_INFO(h.pop());
  TEST(h.size() == 2);
  TEST(h.top() == 5);
  TEST_INFO(h.pop());
  TEST(h.size() == 1);
  TEST(h.top() == 10);
  TEST_INFO(h.pop());
  TEST(h.size() == 0);
}

template<typename heap_T>
void test_mutable_min_heap(heap_T&& h, const char* name) {
  using namespace std;

  TEST_INFO(auto x1 = h.push(1));
  TEST(*x1 == 1);
  TEST_INFO(auto x2 = h.push(2));
  TEST(*x2 == 2);
  TEST_INFO(auto x10 = h.push(10));
  TEST(*x10 == 10);
  TEST_INFO(auto x5 = h.push(5));
  TEST(*x5 == 5);
  TEST(h.top() == *x1);
  TEST_INFO(*x10 = 0);
  TEST_INFO(h.maintain_towards_top(x10));
  TEST(h.top() == *x10);
  TEST_INFO(*x10 = 10);
  TEST_INFO(h.maintain_towards_bottom(x10));
  TEST(h.top() == *x1);
  TEST_INFO(*x5 = 0);
  TEST_INFO(h.maintain_update(x5));
  TEST(h.top() == *x5);
}


int main(const int argc, const char** argv) {
  using namespace std;
  using namespace com_masaers;
  
  // test_max_heap(make_mutable_max_heap<int>(),
  //       	"make_mutable_max_heap<T>()");
  // test_max_heap(make_mutable_max_heap<int>(less<int>()),
  //       	"make_mutable_max_heap<T>(less<T>())");
  // test_max_heap(make_mutable_max_heap<int, vector>(),
  //       	"make_mutable_max_heap<T, vector>()");
  // test_max_heap(make_mutable_max_heap<int, vector>(less<int>()),
  //       	"make_mutable_max_heap<T, vector>(less<T>())");
  test_min_heap(make_mutable_min_heap<int>(),
		"make_mutable_min_heap<T>()");
  test_min_heap(make_mutable_min_heap<int>(less<int>()),
		"make_mutable_min_heap<T>(less<T>())");
  test_min_heap(make_mutable_min_heap<int, vector>(),
		"make_mutable_min_heap<T, vector>()");
  test_min_heap(make_mutable_min_heap<int, vector>(less<int>()),
		"make_mutable_min_heap<T, vector>(less<T>())");

  test_mutable_min_heap(make_mutable_min_heap<int>(),
			"make_mutable_min_heap<T>()");
  test_mutable_min_heap(make_mutable_min_heap<int>(less<int>()),
			"make_mutable_min_heap<T>(less<T>())");
  test_mutable_min_heap(make_mutable_min_heap<int, vector>(),
			"make_mutable_min_heap<T, vector>()");
  test_mutable_min_heap(make_mutable_min_heap<int, vector>(less<int>()),
			"make_mutable_min_heap<T, vector>(less<T>())");

  return EXIT_SUCCESS;
}

/******************************************************************************/
