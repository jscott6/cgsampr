

#include "weighted_graph.h"

// [[Rcpp::plugins(cpp11)]]

RCPP_MODULE(WeightedGraph) {
  using namespace Rcpp;
  class_<WeightedGraph>("WeightedGraph")
  .constructor<IntegerMatrix,IntegerMatrix>()
  .method("printData", &WeightedGraph::printData, "Prints internal data structure to R console")
  .method("sampleStep", &WeightedGraph::sampleStep, "Performs a single sampling step")
  .property("adjacency_matrix", &WeightedGraph::adjacency_matrix, "Returns graph biadjacency matrix")
  ;
}
