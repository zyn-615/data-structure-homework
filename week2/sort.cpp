#include <bits/stdc++.h>

template <std::random_access_iterator I,
          class Comp = std::ranges::less,
          class Proj = std::identity>
requires std::sortable<I, Comp, Proj>
void quicksort(I first, I last, Comp comp = {}, Proj proj = {}) {
  while (last - first > 1) {
    I left  = first;
    I right = last - 1;
    const auto pivot_key = std::invoke(proj, *(first + (last - first) / 2)); 

    while (true) {
      while (left  < last  && comp(std::invoke(proj, *left),  pivot_key)) ++left;
      while (right >= first && comp(pivot_key, std::invoke(proj, *right))) --right;
      if (!(left < right)) break;
      std::iter_swap(left++, right--);
    }
    I mid = right + 1;

    if (mid - first < last - mid) {
      quicksort(first, mid, comp, proj);
      first = mid;
    } else {
      quicksort(mid, last, comp, proj);
      last = mid;
    }
  }
}
