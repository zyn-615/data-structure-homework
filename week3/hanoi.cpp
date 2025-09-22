#include <bits/stdc++.h>

using u64 = unsigned long long;
using i64 = long long;

void hanoi_iter_gray(unsigned n, std::array<char, 3> names = {'A','B','C'}) {
  if (n == 0) return;

  std::vector<int> pos(n + 1, 0);
  const int dir_odd = (n % 2 == 0) ? 1 : 2;
  const int dir_even = 3 - dir_odd;
  const u64 total = (1ULL << n) - 1;
  std::cout << "total moves = " << total << '\n';
  for (auto m : std::views::iota(1ULL, total + 1)) {
    const unsigned tz = std::countr_zero(m);
    const int k = static_cast<int>(tz) + 1;
    const int dir = (k & 1) ? dir_odd : dir_even;
    const int from = pos[k];
    const int to = (from + dir) % 3;
    std::cout << "move disk " << k << ": " << names[from] << " -> " << names[to] << '\n';
    pos[k] = to;
  }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  unsigned n;
  if (!(std::cin >> n)) return 0;
  hanoi_iter_gray(n, {'A','B','C'});
  
  return 0;
}
