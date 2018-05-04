#ifndef GUARD_WeightedGraph
#define GUARD_WeightedGraph

#include <Rcpp.h>
#include <random>
#include "edge.h"

struct DeltaRange {
  int low, up;
  DeltaRange() : low(INT_MIN), up(INT_MAX) {};
};

class WeightedGraph {
public:
  WeightedGraph(Rcpp::IntegerMatrix x, Rcpp::IntegerMatrix f);
  void sampleKernel(std::vector<Weighted::Edge*> & vec);
  DeltaRange getDeltaRange(std::vector<Weighted::Edge*> & vec);
  void updateWeights(std::vector<Weighted::Edge*>& vec, int delta);
  void printData();
  void sampleStep();
  Rcpp::IntegerMatrix adjacency_matrix() { return adjacency_matrix_; };
  Weighted::Edge** edges() { return edges_; };

private:
  std::vector<Weighted::Vertex> vertices_;
  Weighted::Edge** edges_;
  std::vector<Weighted::Vertex*> initial_vertices_;
  Rcpp::IntegerMatrix adjacency_matrix_;
  std::default_random_engine generator_;
};

#endif
