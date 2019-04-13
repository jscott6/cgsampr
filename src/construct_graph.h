
/*
Module implements the Edmond-Karps algorithm for for finding a viable graph
*/


#ifndef GUARD_construct_graph
#define GUARD_construct_graph

#include <vector>
#include <queue>
#include <Rcpp.h>

namespace ConstructGraph {

    using IV = Rcpp::IntegerVector;
    using IM = Rcpp::IntegerMatrix;

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
public:
    Graph(IV, IV, IM);
    void addEdge(int, int, int);
    bool findPath();
    int calcPathFlow();
    void updateFlow(int);
    IM constructWeightMatrix();

private:
    std::vector<Vertex> vertices_;
    std::vector<std::vector<int> > adjacency_list_, flow_, capacity_;
    unsigned int source_, sink_;
    int m_, n_;
  };
}

#endif