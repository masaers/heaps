#include "binary_Heap.hpp"
#include <vector>
#include <iostream>
#include <cstdlib>

int main(const int argc, const char** argv) {
  using namespace com_masaers;
  using namespace std;

  auto bh = make_binary_heap<int>();
  const auto print_bh = [&]() -> ostream& {
    cout << '[';
    for (const auto& e : bh) {
      cout << ' ' << e->value_m;
    }
    cout << " ]";
    return cout;
  };
  
  for (int i = 10; i > 0; --i) {
    bh.push(i);
    print_bh() << endl;
  }
  cout << endl;
  for (auto h : bh) {
    if (bh.value(h) % 2 == 0) {
      bh.update(h, bh.value(h) + 2);
      print_bh() << endl;
    }
  }
  cout << endl;
  for (auto h : bh) {
    if (bh.value(h) % 2 == 0) {
      bh.ensure_priority(h, 2);
      print_bh() << endl;
    }
  }
  cout << endl;
  while (! bh.empty()) {
    print_bh() << endl;
    bh.pop();
  }
  
  return EXIT_SUCCESS;
}

