#include <07_quicksort/inc/easy.h>
#include <iostream>

int main() {
  namespace fc = frozenca;
  using namespace std;

  {
    vector<int> A{27, 17, 3, 16, 13, 10, 1, 5, 7, 12, 4, 8, 9, 0};
    fc::quicksort(A);
    fc::print(A);
    if (ranges::is_sorted(A)) {
      cout << "Sorted\n";
    } else {
      cout << "Failed to sort\n";
    }
  }
  {
    vector<int> A{27, 17, 3, 16, 13, 10, 1, 5, 7, 12, 4, 8, 9, 0};
    fc::quicksort_decreasing(A);
    fc::print(A);
    if (ranges::is_sorted(A, ranges::greater{})) {
      cout << "Sorted in decreasing order\n";
    } else {
      cout << "Failed to sort\n";
    }
  }
}