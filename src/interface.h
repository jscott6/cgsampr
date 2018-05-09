#ifndef GUARD_interface
#define GUARD_interface

#include <Rcpp.h>
#include <vector>
#include <random>

#define pure = 0;
using IM = Rcpp::IntegerMatrix;
using IV = Rcpp::IntegerVector;

struct Vertex {
  int index;
}

class Edge {
public:
  Edge(Vertex* const head, Vertex* const tail, int const fixed, int* const weight)
    : head_(head), tail_(tail), fixed_(fixed), weight_(weight) { }
  Vertex* const head_, * const tail_;
  int const fixed_;
  int* const weight_;
}

class Graph {
public:
  Graph(IM adjacency_matrix, IM fixed);
  Graph(IV in_degree, IV out_degree, IM fixed);
  Rcpp::List sample(int nsamples = 10000, int thin = 10, int burnin = 5000);
  void summary();
  IM adjacency_matrix() { return adjacency_matrix_; }
  IM fixed() { return fixed_; }
private:
  std::default_random_engine generator_;
  IM adjacency_matrix_, fixed_;
  virtual void updateAdjacencyMatrix() pure;
  virtual void sampleStep() pure;
};


namespace Directed {

  template <typename V, typename E>
  class Graph : public ::Graph {
  public:
    Graph(IM adjacency_matrix, IM fixed);
    Graph(IV in_degrees, IV out_degrees, IM fixed);
  private:
    std::vector<::V> vertices_;
    std::vector<std::vector<::E> > edges_;
  };
}

  template <typename V, typename E>
namespace Undirected {

  class Graph : public ::Graph {
  public:
    Graph(IM adjacency_matrix, IM fixed);
    Graph(IV in_degrees, IV out_degrees, IM fixed);
  private:
    std::vector<::V> vertices_;
    std::vector<std::vector<::E> > edges_;
  };
}
