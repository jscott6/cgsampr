#ifndef GUARD_WeightedGraph
#define GUARD_WeightedGraph

#include <Rcpp.h>
#include <random>
#include "edge.h"
#include "directed_graph.h"

namespace Weighted {

  using IM = Rcpp::IntegerMatrix;
  using IV = Rcpp::IntegerVector;

  struct DeltaRange{
    int low, up;
  };

  namespace Directed {

    class Graph : public ::Directed::Graph<::Weighted::Vertex, ::Weighted::Edge> {
    public:
      Graph(IM adjacency_matrix, IM fixed);
      void sampleKernel(std::vector<::Weighted::Edge*>& vec);
      ::Weighted::DeltaRange getDeltaRange(std::vector<::Weighted::Edge*> & vec);
      void updateWeights(std::vector<::Weighted::Edge*>& vec, int delta);
      void sampleStep() override;
      void printData();
      ::Weighted::Edge** edges() { return edges_; }
    private:
      std::vector<::Weighted::Vertex*> initial_vertices_;
      void updateAdjacencyMatrix() override;
    };
  }
}




#endif
