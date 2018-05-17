#ifndef GUARD_Edge
#define GUARD_Edge

#include "interface.h"
#define STAR INT_MAX

namespace Weighted
{

class Edge;

struct Vertex : public Base::Vertex
{
  std::vector<Edge *> in_edges;
  std::vector<Edge *> out_edges;
  uint even, even_zeros, zeros;
  void reset()
  {
    even = 0;
    even_zeros = 0;
    zeros = 0;
  }
};

class Edge : public Base::Edge<Vertex>
{
public: // interface
  Edge(Vertex *const head, Vertex *const tail, int *const weight, const bool fixed);
  void increment(), decrement();
  // getters and setters
  void reset() { even_ = STAR; odd_ = STAR; };
  void weight(int);
  int pos() { return pos_; };
  int weight() { return *weight_; };
  int even() { return even_; };
  int odd() { return odd_; };

private:
  int pos_, even_, odd_;
  void add();
  void remove();
};
} // namespace Weighted

#endif
