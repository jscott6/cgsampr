

#include "weighted_graph.h"
using namespace Weighted::Directed;

RCPP_MODULE(WeightedGraph_module) {
  using namespace Rcpp;
  class_<Graph>("WeightedDirectedGraph")
  .constructor<IntegerMatrix,IntegerMatrix>()
  .method("printData", &Graph::printData, "Prints internal data structure to R console")
  .method("sampleStep", &Graph::sampleStep, "Performs a single sampling step")
  //.property("adjacency_matrix", &Graph::adjacency_matrix, "Returns Graph biadjacency matrix")
  ;
}
