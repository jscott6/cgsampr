#ifndef GUARD_WeightedGraph
#define GUARD_WeightedGraph

#include <Rcpp.h>
#include <random>
#include "interface.h"

using IM = Rcpp::IntegerMatrix;
using IV = Rcpp::IntegerVector;

namespace Weighted {

  struct DeltaRange{
    int low, up;
  };

  class Edge;

  struct Vertex : public ::Vertex {
    std::vector<Edge*> in_edges;
    std::vector<Edge*> out_edges;
  }

  class Edge : public ::Edge {
    public: // interface
      Edge(Vertex* const head, Vertex* const tail, int* const weight, const bool fixed);
      void increment();
      void decrement();
      // getters and setters
      void reset() { visits_ = STAR; };
      const Vertex* tail() { return tail_; };
      const Vertex* head() { return head_; };
      const bool fixed() { return fixed_; };
      void weight(int);
      int pos() { return pos_; };
      int weight() { return *weight_; };
      int visits() { return visits_; };
    private:
      int pos_, visits_;
      void add();
      void remove();
  };

  namespace Directed {

    class Graph : Directed::Graph {
    public:
      Graph(IM adjacency_matrix, IV fixed);
      void sampleKernel(std::vector<Weighted::Edge*>& vec);
      Weighted::DeltaRange getDeltaRange(std::vector<Weighted::Edge*> & vec);
      void updateWeights(std::vector<Weighted::Edge*>& vec, int delta);
      void printData();
      Weighted::Edge** edges() { return edges_; }
    private:
      std::vector<Weighted::Vertex*> initial_vertices_;
    };
  }
}




#endif
