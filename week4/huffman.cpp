#include <bits/stdc++.h>

struct Node {
  long long f;
  int ch;
  std::shared_ptr<Node> l, r;
  Node(long long f_, int c, std::shared_ptr<Node> L=nullptr, std::shared_ptr<Node> R=nullptr)
    : f(f_), ch(c), l(std::move(L)), r(std::move(R)) {}
};

struct Cmp {
  bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const noexcept {
    return a->f > b->f;
  }
};

struct EncodeResult {
  std::string bits;
  std::array<std::string,256> codebook;
};

static void dfs(const std::shared_ptr<Node>& t, const std::string& pre, std::array<std::string,256>& cb) {
  if (!t) return;
  if (t->ch != -1) { cb[(unsigned)t->ch] = pre.empty() ? "0" : pre; return; }
  dfs(t->l, pre + '0', cb);
  dfs(t->r, pre + '1', cb);
}

static std::shared_ptr<Node> build_tree(const std::vector<std::uint8_t>& data) {
  std::array<long long,256> f{};
  for (auto b : data) ++f[b];
  std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, Cmp> pq;
  int kinds = 0;
  for (int c = 0; c < 256; ++c) if (f[c]) { pq.push(std::make_shared<Node>(f[c], c)); ++kinds; }
  if (kinds == 0) return nullptr;
  if (kinds == 1) {
    auto leaf = pq.top(); pq.pop();
    pq.push(std::make_shared<Node>(leaf->f, -1, leaf, std::make_shared<Node>(0, -1)));
  }
  while (pq.size() > 1) {
    auto a = pq.top(); pq.pop();
    auto b = pq.top(); pq.pop();
    pq.push(std::make_shared<Node>(a->f + b->f, -1, a, b));
  }
  return pq.top();
}

EncodeResult huffman_encode(const std::vector<std::uint8_t>& data) {
  EncodeResult r;
  for (auto& s : r.codebook) s.clear();
  auto root = build_tree(data);
  if (!root) return r;
  dfs(root, "", r.codebook);
  r.bits.reserve(data.size());
  for (auto b : data) r.bits += r.codebook[b];
  return r;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  std::string text = "huffman coding made simple";
  std::vector<std::uint8_t> bytes(text.begin(), text.end());
  auto enc = huffman_encode(bytes);

  std::cout << enc.bits.size() << "\n";
  std::cout << enc.bits.substr(0, 128) << (enc.bits.size() > 128 ? "..." : "") << "\n";
  
  return 0;
}
