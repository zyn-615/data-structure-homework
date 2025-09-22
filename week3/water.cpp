#include <bits/stdc++.h>

using u64 = unsigned long long;
using i64 = long long;

struct State { int a, b; };
struct Step  { std::string_view op; State after; };
using Plan = std::vector<Step>;

std::optional <Plan> solve(int cap_a, int cap_b, int target) {
  if (target == 0) return Plan{};
  if (target > std::max(cap_a, cap_b)) return std::nullopt;
  int g = std::gcd(cap_a, cap_b);
  if (g == 0 || target % g != 0) return std::nullopt;

  const int W = cap_b + 1, N = (cap_a + 1) * (cap_b + 1);
  auto id = [&](int x, int y){ return x * W + y; };
  auto from = [&](int u) -> State { return {u / W, u % W}; };

  std::vector<char> vis(N, 0);
  std::vector<int> prev(N, -1);
  std::vector<std::string_view> how(N);
  std::queue<int> q;

  int start = id(0, 0);
  vis[start] = 1; q.push(start);

  int goal = -1;
  while (!q.empty()) {
    int u = q.front(); q.pop();
    auto [x, y] = from(u);
    if (x == target || y == target) { goal = u; break; }

    auto push = [&](int nx, int ny, std::string_view op) {
      int v = id(nx, ny);
      if (vis[v]) return;
      vis[v] = 1; prev[v] = u; how[v] = op; q.push(v);
    };

    if (x < cap_a) push(cap_a, y, "fill A");
    if (y < cap_b) push(x, cap_b, "fill B");
    if (x > 0)     push(0, y,     "empty A");
    if (y > 0)     push(x, 0,     "empty B");
    if (x > 0 && y < cap_b) {
      int pour = std::min(x, cap_b - y);
      push(x - pour, y + pour, "pour A->B");
    }
    if (y > 0 && x < cap_a) {
      int pour = std::min(y, cap_a - x);
      push(x + pour, y - pour, "pour B->A");
    }
  }

  if (goal < 0) return std::nullopt;

  Plan path;
  for (int u = goal; u != start; u = prev[u]) path.push_back({how[u], from(u)});
  std::reverse(path.begin(), path.end());
  return path;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  int a, b, t;
  if (!(std::cin >> a >> b >> t)) return 0;

  if (auto plan = solve(a, b, t)) {
    std::cout << "steps = " << plan->size() << '\n';
    int i = 0;
    for (const auto& s : *plan)
      std::cout << ++i << ". " << s.op << " -> (" << s.after.a << ',' << s.after.b << ")\n";
  } else {
    std::cout << "impossible\n";
  }
  return 0;
}
