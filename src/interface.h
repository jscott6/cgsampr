#ifndef GUARD_interface
#define GUARD_interface

#include <Rcpp.h>
#include <vector>
#include <random>

#define pure = 0;


namespace Base {

  using IM = Rcpp::IntegerMatrix;
  using IV = Rcpp::IntegerVector;

  struct Vertex {
    int index;
  };

  template <typename V>
  class Edge {
  public:
    Edge(V* const head, V* const tail, int* const weight,  int const fixed)
      : head_(head), tail_(tail), weight_(weight), fixed_(fixed) { }
    V* const head_, * const tail_;
    int* const weight_;
    int const fixed_;
  };

  class Graph {
  public:
    Graph(IM adjacency_matrix, IM fixed);
    Graph(IV in_degree, IV out_degree, IM fixed);
    virtual ~Graph() { }
    Rcpp::List sample(int nsamples = 10000, int thin = 10, int burnin = 5000);
    void summary();
    virtual void sampleStep() pure;
    IM adjacency_matrix() { return adjacency_matrix_; }
    IM fixed() { return fixed_; }
  protected:
    IM adjacency_matrix_, fixed_;
    std::default_random_engine generator_;
    virtual void updateAdjacencyMatrix() pure;
  };



}


#endif
