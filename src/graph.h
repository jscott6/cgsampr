#ifndef GUARD_UnweightedGraph
#define GUARD_UnweightedGraph

#include <Rcpp.h>
#include <random>
#include "edge.h"
#include "directed_graph.h"

namespace Unweighted
{
using IM = Rcpp::IntegerMatrix;
using IV = Rcpp::IntegerVector;

class Edge;

struct Vertex : public Base::Vertex
{
    std::vector<Edge *> in_edges;
    std::vector<Edge *> not_out_edges;
};

class Edge : public Base::Edge<Vertex>
{
public: // interface
  Edge(Vertex *const head, Vertex *const tail, int *const weight, const bool fixed);
  void weight(int w) { *weight_ = w; }
  int weight() const { return *weight_; }  
};

namespace Directed
{
class Graph : public ::Directed::Graph<::Unweighted::Vertex, ::Unweighted::Edge>
{
  public:
    Graph(IM adjacency_matrix, IM fixed);
    Graph(IV in_degree, IV out_degree, IM fixed);
    void printData() const;
    void sampleStep() override;

  private:
    void updateAdjacencyMatrix() override;
    std::vector<Unweighted::Vertex*> initial_vertices_;
};
} // namespace Directed
} // namespace Unweighted

#endif





