#ifndef GUARD_interface
#define GUARD_interface

#include <Rcpp.h>
#include <vector>
#include <random>

#define pure = 0;

class GraphBase {
using V = AbstractVertex;
using E = AbstractEdge;
using IM = Rcpp::IntegerMatrix;
using IV = Rcpp::IntegerVector;
public:
  AbstractGraph(IM adjacency_matrix, IM fixed);
  AbstractGraph(IV in_degrees, IV out_degrees, IM f);
  class EdgeBase {
  using V = AbstractVertex;
  public:
    AbstractEdge(V* const head, V* const tail, int const fixed)
      : head_(head), tail_(tail) fixed_(fixed) {}
    V* const head_, V* const tail_;
    int const fixed;
  }
  struct VertexBase {
    int index;
  }
  Rcpp::List sample(int nsamples = 10000, int thin = 10, int burnin = 5000);
  void summary();
  // getter methods
  IM adjacency_matrix() { return adjacency_matrix_; };
  IM fixed() { return fixed_; };
private:
  std::default_random_engine generator_;
  IM adjacency_matrix_, fixed_;
  virtual void updateAdjacencyMatrix() pure;
  virtual void sampleStep() pure;
};



class DirectedGraph : public GraphBase {
using IM = Rcpp::IntegerMatrix;
using IV = Rcpp::IntegerVector;
public:
  DirectedGraph(IM adjacency_matrix, IM fixed);
  DirectedGraph(IV in_degrees, IV out_degrees, IM f);
private:
  std::vector<AbstractVertex> vertices_;
  std::vector<std::vector<Edge> > edges_;
};

class UndirectedGraph : public GraphBase {
using IM = Rcpp::IntegerMatrix;
using IV = Rcpp::IntegerVector;
public:
  UndirectedGraph(IM adjacency_matrix, IM fixed);
  UndirectedGraph(IV in_degrees, IV out_degrees, IM f);
private:
  std::vector<AbstractVertex> vertices_;
  std::vector<std::vector<Edge> > edges_;
};


class WeightedEdge : public EdgeBase {
using V = AbstractVertex;
public:
  WeightedEdge(V* const head, V* const tail, int const fixed, int * const weight)
    : V(head, tail, fixed),
      weight_(weight) {}
  int * const weight_;
}

class UnweightedDirectedGraph : public DirectedGraph {

}





class WeightedDirectedGraph : public DirectedGraph {}


namespace Unweighted {
  namespace Directed {
    struct Vertex: AbstractVertex {
      std::vector<Edge *> in_edges;
      std::vector<Edge *> not_out_edges;
    }
    class Graph : public Directed::Graph {

    };
  }
  namespace Undirected {
    class Graph : public Undirected::Graph {

    };
  }
}

namespace Weighted {
  namespace Directed {
    class Graph : public Directed::Graph {

    };
  }
  namespace Undirected {
    class Graph : public Undirected::Graph {

    };
  }
}
