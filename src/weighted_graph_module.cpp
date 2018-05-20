

#include "weighted_graph.h"

RCPP_MODULE(WeightedGraph_module)
{
  using namespace Rcpp;

  class_<Base::Graph>("BaseGraph")
      .constructor<IntegerMatrix, IntegerMatrix, bool>()
      .method("sample", &Base::Graph::sample, "Performs multiple sampling steps with optional thinning and burnin period")
      .property("adjacency_matrix", &Base::Graph::adjacency_matrix, "Returns Graph biadjacency matrix")
      .property("fixed", &Base::Graph::fixed, "Returns binary matrix specifying fixed edges/non-edges");
  class_<Weighted::Directed::Graph>("WeightedGraph")
      .derives<Base::Graph>("BaseGraph")
      .constructor<IntegerMatrix, IntegerMatrix>()
      .const_method("printData", &Weighted::Directed::Graph::printData, "Prints internal data structure to R console")
      .method("sampleStep", &Weighted::Directed::Graph::sampleStep, "Performs a single sampling step");
}
