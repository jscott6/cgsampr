#ifndef GUARD_Edge
#define GUARD_Edge

#include "interface.h"

namespace Weighted
{

class Edge;

struct Vertex : public Base::Vertex
{
  std::vector<Edge *> in_edges;
  std::vector<Edge *> out_edges;
  int n_up, n_low, n_int;
  void reset()
  {
    n_up = in_edges.size();
    n_low = n_up;
    n_int = n_up;
  };
};

class Edge : public Base::Edge<Vertex>
{
public: // interface
  Edge(Vertex *const head, Vertex *const tail, int *const weight, const bool fixed);
  void registerVisit() { visited_ = true; }
  // getters and setters
  void reset() { visited_ = false; }
  void weight(int);
  int pos() const { return pos_; }
  int weight() const { return *weight_; }
  bool visited() const { return visited_; }

private:
  int pos_;
  bool visited_;
  void add();
  void remove();
};
} // namespace Weighted

#endif
