#ifndef GUARD_WeightedGraph
#define GUARD_WeightedGraph

#include <Rcpp.h>
#include <random>
#include "edge.h"
#include "directed_graph.h"

namespace Weighted
{

using IM = Rcpp::IntegerMatrix;
using IV = Rcpp::IntegerVector;

struct DeltaRange
{
  int low, up;
  DeltaRange()
      : low(INT_MIN), up(INT_MAX)
  {
  }
};

struct Factor
{
  double low, up;
  Factor()
      : low(1.), up(1.)
  {
  }
  Factor(double low, double up)
      : low(low), up(up)
  {
  }
};

namespace Directed
{

class Graph : public ::Directed::Graph<::Weighted::Vertex, ::Weighted::Edge>
{
public:
  Graph(IM adjacency_matrix, IM fixed);
  int sampleKernel(std::vector<::Weighted::Edge *> &vec);
  ::Weighted::DeltaRange getDeltaRange(std::vector<::Weighted::Edge *> &vec);
  void updateWeights(std::vector<::Weighted::Edge *> &vec, int delta);
  void sampleStep() override;
  void printData() const;
  ::Weighted::Edge **edges() { return edges_; }
  Factor getBoundaryWeights(const std::vector<Weighted::Edge *> &vec, const Weighted::DeltaRange& dr);

private:
  std::vector<::Weighted::Vertex *> initial_vertices_;
  void updateAdjacencyMatrix() override;
};
} // namespace Directed

} // namespace Weighted


#endif
