#ifndef GUARD_Edge
#define GUARD_Edge

#include "interface.h"
#define STAR 0

namespace Weighted
{

class Edge;

struct CycleData
{
  int visits;
  int up_zeros, up_zero_visits;
  int low_zeros, low_zero_visits;
  CycleData()
      : visits(0), up_zeros(0), up_zero_visits(0), low_zeros(0), low_zero_visits(0)
  {
  }
  CycleData(int visits, int up_zeros, int up_zero_visits, int low_zeros, int low_zero_visits)
      : visits(visits), up_zeros(up_zeros), up_zero_visits(up_zero_visits), low_zeros(low_zeros), low_zero_visits(low_zero_visits)
  {
  }
  void reset()
  {
    visits = 0;
    up_zeros = 0;
    up_zero_visits = 0;
    low_zeros = 0;
    low_zero_visits = 0;
  }
};

struct Vertex : public Base::Vertex
{
  std::vector<Edge *> in_edges;
  std::vector<Edge *> out_edges;
  CycleData cycle_data;
};

class Edge : public Base::Edge<Vertex>
{
public: // interface
  Edge(Vertex *const head, Vertex *const tail, int *const weight, const bool fixed);
  void incrementEven() { even_++; }
  void incrementOdd() { odd_++; }
  // getters and setters
  void reset() { even_ = STAR; odd_ = STAR; }
  void weight(int);
  int pos() const { return pos_; }
  int weight() const { return *weight_; }
  int even() const { return even_; }
  int odd() const { return odd_; }

private:
  int pos_, even_, odd_;
  void add();
  void remove();
};
} // namespace Weighted

#endif
