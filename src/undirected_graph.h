
#ifndef GUARD_undirected_graph
#define GUARD_undirected_graph

#include "interface.h"

namespace Undirected {

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
  std::vector<std::vector<E>> edges_;
};

template <typename V, typename E>
void Graph<V, E>::init()
{
  int nrow = adjacency_matrix.nrow();
  vertices_ = std::vector<V>(nrow);
  for (int i = 0; i != nrow; ++i)
    vertices_[i].index = i;
  edges_ = (E **)malloc(nrow * sizeof(E));
  for (int i = 0; i != nrow; ++i)
    edges_[i] = (E *)malloc((nrow - 1 - i) * sizeof(E));
  for (int i = 0; i != nrow; ++i)
  {
    int k = 0;
    for (int j = (i + 1); j != nrow; ++j)
    {
      new (&edges_[i][k]) E(&vertices_[j], &vertices_[i],
                            &adjacency_matrix_(i, j), fixed(i, j));
      k++;
    }
  }
}

template <typename V, typename E>
Graph<V, E>::Graph(IM adjacency_matrix, IM fixed, bool search)
    : Base::Graph(adjacency_matrix, fixed, search)
{
  init();
}

template <typename V, typename E>
Graph<V, E>::Graph(IV in_degree, IV out_degree, IM fixed, bool search) :
    : Base::Graph(in_degree, out_degree, fixed, search)
{
  init();
}

template <typename V, typename E>
Graph<V, E>::~Graph()
{
  delete edges_;
}
}

#endif
