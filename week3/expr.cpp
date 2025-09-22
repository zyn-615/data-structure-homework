#include <bits/stdc++.h>

using i64 = long long;

double eval_expr(std::string_view s) {
  auto apply = [](std::vector<double> & v, std::vector<char> & o) {
    char op = o.back(); o.pop_back();
    if (op == '!') {
      double a = v.back(); v.pop_back();
      v.push_back(-a);
      return;
    }
    double b = v.back(); v.pop_back();
    double a = v.back(); v.pop_back();
    if (op == '+') v.push_back(a + b);
    else if (op == '-') v.push_back(a - b);
    else if (op == '*') v.push_back(a * b);
    else if (op == '/') v.push_back(a / b);
    else v.push_back(std::pow(a, b));
  };

  auto prec = [](char op) {
    if (op == '^') return 4;
    if (op == '!') return 3;
    if (op == '*' || op == '/') return 2;
    return 1;
  };

  auto right_assoc = [](char op) { return op == '^' || op == '!'; };

  std::vector<double> val;
  std::vector<char> op;
  int n = static_cast<int>(s.size());

  auto isop = [](char c) { return c == '+' || c == '-' || c == '*' || c == '/' || c == '^'; };

  int expect_unary = 1;

  for (int i = 0; i < n; ) {
    if (std::isspace(static_cast<unsigned char>(s[i]))) { ++i; continue; }

    if (s[i] == '(') { op.push_back('('); ++i; expect_unary = 1; continue; }

    if (s[i] == ')') {
      while (!op.empty() && op.back() != '(') apply(val, op);
      if (!op.empty() && op.back() == '(') op.pop_back();
      ++i; expect_unary = 0; continue;
    }

    if (isop(s[i])) {
      char c = s[i++];
      if (expect_unary) {
        if (c == '+') { continue; }
        if (c == '-') { op.push_back('!'); continue; }
      }
      while (!op.empty() && op.back() != '(' &&
             (prec(op.back()) > prec(c) ||
              (prec(op.back()) == prec(c) && !right_assoc(c)))) {
        apply(val, op);
      }
      op.push_back(c);
      expect_unary = 1;
      continue;
    }

    if (std::isdigit(static_cast<unsigned char>(s[i])) || s[i] == '.') {
      double x = 0.0, base = 1.0;
      bool has_int = false, has_frac = false;

      while (i < n && std::isdigit(static_cast<unsigned char>(s[i]))) { x = x * 10 + (s[i] - '0'); ++i; has_int = true; }
      if (i < n && s[i] == '.') {
        ++i;
        while (i < n && std::isdigit(static_cast<unsigned char>(s[i]))) { base /= 10.0; x += (s[i] - '0') * base; ++i; has_frac = true; }
      }
      if (!has_int && !has_frac) throw std::runtime_error("number expected");

      if (i < n && (s[i] == 'e' || s[i] == 'E')) {
        ++i;
        int sign = 1;
        if (i < n && (s[i] == '+' || s[i] == '-')) { if (s[i] == '-') sign = -1; ++i; }
        int expv = 0;
        if (i >= n || !std::isdigit(static_cast<unsigned char>(s[i]))) throw std::runtime_error("bad exponent");
        while (i < n && std::isdigit(static_cast<unsigned char>(s[i]))) { expv = expv * 10 + (s[i] - '0'); ++i; }
        x = x * std::pow(10.0, sign * expv);
      }

      val.push_back(x);
      expect_unary = 0;
      continue;
    }

    throw std::runtime_error("invalid character");
  }

  while (!op.empty()) {
    if (op.back() == '(') throw std::runtime_error("mismatched (");
    apply(val, op);
  }
  if (val.size() != 1) throw std::runtime_error("parse error");
  return val.back();
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  std::string expr;
  if (!std::getline(std::cin, expr)) return 0;

  std::cout.setf(std::ios::fixed);
  std::cout << std::setprecision(12) << eval_expr(expr) << '\n';
  return 0;
}
