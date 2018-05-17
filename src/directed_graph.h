
#ifndef GUARD_directed_graph
#define GUARD_directed_graph

#include "interface.h"

namespace Directed
{

using IM = Rcpp::IntegerMatrix;
using IV = Rcpp::IntegerVector;

template <typename V, typename E>
class Graph : public Base::Graph
{
public:
  Graph(IM adjacency_matrix, IM fixed, bool search);
  Graph(IV in_degrees, IV out_degrees, IM fixed, bool search);
  ~Graph();

protected:
  void init();
  std::vector<V> vertices_;
  E **edges_;
};

template <typename V, typename E>
void Graph<V, E>::init()
{
  int nrow = adjacency_matrix_.nrow();
  int ncol = adjacency_matrix_.ncol();
  vertices_ = std::vector<V>(nrow + ncol);
  for (int i = 0; i != nrow + ncol; ++i)
    vertices_[i].index = i;
  // allocate memory WITHOUT calling constructor
  edges_ = (E **)malloc(nrow * sizeof(E));
  for (int i = 0; i != nrow; ++i)
    edges_[i] = (E *)malloc(ncol * sizeof(E));
  // initialise edges
  // applies constructor directly to final address (avoids copy constructor)
  // important to keep correct pointers in Vertex structures
  for (int i = 0; i != nrow; ++i)
    for (int j = 0; j != ncol; ++j)
      new (&edges_[i][j]) E(&vertices_[nrow + j], &vertices_[i],
                            &adjacency_matrix_(i, j), fixed_(i, j));
}

template <typename V, typename E>
Graph<V, E>::Graph(IM adjacency_matrix, IM fixed, bool search)
    : Base::Graph(adjacency_matrix, fixed, search)
{
  init();
}

template <typename V, typename E>
Graph<V, E>::Graph(IV in_degree, IV out_degree, IM fixed, bool search)
    : Base::Graph(in_degree, out_degree, fixed, search)
{
  init();
}

template <typename V, typename E>
Graph<V, E>::~Graph()
{
  delete edges_;
}
} // namespace Directed

#endif
