#include <bits/stdc++.h>
#include "list.h"

using i64 = long long;

constexpr double eps = 1e-8;
inline int dcmp(double a) {
  if (fabs(a) < eps) return 0;
  return a < 0 ? -1 : 1;
}

struct Point {
  double x, y;
  Point() {}
  Point(double a, double b) : x(a), y(b) {}
};

typedef Point Vector;
Vector operator +(Vector A, Vector B) {
  return Vector(A.x + B.x, A.y + B.y);
}

Vector operator -(Vector A, Vector B) {
  return Vector(A.x - B.x, A.y - B.y);
}

Vector operator *(Vector A, double p) {
  return Vector(A.x * p, A.y * p);
}

Vector operator /(Vector A, double p) {
  return Vector(A.x / p, A.y / p);
}

inline double dot(Vector A, Vector B) {
  return A.x * B.x + A.y * B.y;
}

inline double cross(Vector A, Vector B) {
  return A.x * B.y - A.y * B.x;
}

inline double length(Vector A) {
  return sqrt(dot(A, A));
}

inline double angle(Vector A, Vector B) {
  return acos(dot(A, B) / length(A) / length(B));
}

inline double dist(Point a, Point b) {
  return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

inline Vector rotate(Point a, double t) {
  return Vector(a.x * cos(t) - a.y * sin(t), a.x * sin(t) + a.y * cos(t));
}

std::vector<Point> Graham(std::vector<Point> points){
  int N = points.size();
  if (N <= 1) return points;

  int pivot = 0;
  for(int i = 1; i < N; ++i) {
    if (dcmp(points[i].y - points[pivot].y) < 0 || (dcmp(points[i].y - points[pivot].y) == 0 && dcmp(points[i].x - points[pivot].x) < 0)) {
      pivot = i;
    }
  }
    
  std::swap(points[0], points[pivot]);
  Point O = points[0];
  
  std::sort(points.begin() + 1, points.end(), [&](const Point& a, const Point& b){
      Vector A = a - O, B = b - O;
      int c = dcmp(cross(A, B));
      if (c != 0) return c > 0;
      return dcmp(length(A) - length(B)) < 0; 
  });
  
  LinkedStack<Point> st;
  st.push(points[0]);
  st.push(points[1]);
  for(int i = 2; i < N; ++i) {
    while(st.size() >= 2 && dcmp(cross(st.second() - st.top(), points[i] - st.top())) <= 0) {
      Point trash; st.pop(trash);
    }
    
    st.push(points[i]);
  }

  std::vector <Point> hull;
  Point t;
  
  while(st.pop(t)) hull.push_back(t);
  reverse(hull.begin(), hull.end());

  return hull;
}
