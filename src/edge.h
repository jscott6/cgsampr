#ifndef GUARD_Edge
#define GUARD_Edge

#include "interface.h"
#define STAR INT_MAX

namespace Weighted {

  class Edge;

  struct Vertex : public Base::Vertex {
    std::vector<Edge*> in_edges;
    std::vector<Edge*> out_edges;
  };

  class Edge : public Base::Edge<Vertex> {
    public: // interface
      Edge(Vertex* const head, Vertex* const tail, int* const weight, const bool fixed);
      void increment(), decrement();
      // getters and setters
      void reset() { visits_ = STAR; };
      void weight(int);
      int pos() { return pos_; };
      int weight() { return *weight_; };
      int visits() { return visits_; };
    private:
      int pos_, visits_;
      void add();
      void remove();
  };
}

#endif
