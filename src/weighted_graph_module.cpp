

#include "weighted_graph.h"

// [[Rcpp::plugins(cpp11)]]

RCPP_MODULE(WeightedGraph_module) {
  using namespace Rcpp;
  using namespace Weighted;
  class_<Graph>("WeightedGraph")
  .constructor<IntegerMatrix,IntegerMatrix>()
  .method("printData", &Graph::printData, "Prints internal data structure to R console")
  .method("sampleStep", &Graph::sampleStep, "Performs a single sampling step")
  .property("adjacency_matrix_", &Graph::adjacency_matrix, "Returns Graph biadjacency matrix")
  ;
}
