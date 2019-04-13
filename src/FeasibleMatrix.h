
#ifndef GUARD_FeasibleMatrix
#define GUARD_FeasibleMatrix

#include <Rcpp.h>
#include <vector>
#include <queue>

namespace FeasibleMatrix {

  using IM = Rcpp::IntegerMatrix;
  using IV = Rcpp::IntegerVector;

  enum VertexColor{white, gray, black};

  struct Vertex {
    unsigned int distance;
    VertexColor color;
    unsigned int predecessor;
    Vertex() : 
      distance(0),
      color(white),
      predecessor(0) {};
  };

  class Graph {
    std::vector<Vertex> vertices_;
    std::vector<std::vector<unsigned int> > adjacency_matrix_, flow_, capacity_;
    unsigned int source_, sink_;
  public:
    Graph(IV, IV, IM);
    void addEdge(unsigned int, unsigned int, unsigned int);
    bool findPath();
    unsigned int calcPathFlow();
    void updateFlow(unsigned int);
    Rcpp::IntegerMatrix constructMatrix(IV, IV);
  };
}

#endif
