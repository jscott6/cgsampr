#ifndef GUARD_Edge
#define GUARD_Edge

#include <vector>

#define STAR INT_MAX

namespace Weighted {

  class Edge;

  struct Vertex {
    int index;
    std::vector<Edge*> in_edges;
    std::vector<Edge*> out_edges;
  };


class AbstractEdge {
using V = AbstractVertex;
public:
  AbstractEdge(V* const head, V* const tail, int const fixed)
    : head_(head), tail_(tail) fixed_(fixed) {}
  V* const head_, V* const tail_;
  int const fixed;
}


namespace UnWeighted {
  namespace Directed {
    class Edge : public AbstractEdge {
    public:
      Edge(Vertex* const head, Vertex* const tail, int const fixed)
        : AbstractEdge(head, tail, fixed) {}
    };
  }
  namespace Undirected {
    class Edge : public AbstractEdge {
    public:

    }
  }
}




  class Edge {
  public: // interface
    Edge(Vertex* const h, Vertex* const t, int* const w, const bool f);
    void increment();
    void decrement();
    // getters and setters
    void reset() { visits_ = STAR; };
    const Vertex* tail() { return tail_; };
    const Vertex* head() { return head_; };
    const bool fixed() { return fixed_; };
    void weight(int);
    int pos() { return pos_; };
    int weight() { return *weight_; };
    int visits() { return visits_; };

  private: // implementation
    Vertex* const head_, *const tail_;
    bool const fixed_;
    int* const weight_;
    int pos_, visits_;
    void add();
    void remove();
  };
}

#endif
