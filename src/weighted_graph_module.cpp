

#include "weighted_graph.h"
#include "graph.h"

RCPP_MODULE(base_module)
{
  using namespace Rcpp;

  class_<Base::Graph>("Base")
      .constructor<IntegerMatrix, IntegerMatrix, bool>()
      .method("sample", &Base::Graph::sample, "Performs multiple sampling steps with optional thinning and burnin period")
      .property("adjacency_matrix", &Base::Graph::adjacency_matrix, "Returns Graph biadjacency matrix")
      .property("fixed", &Base::Graph::fixed, "Returns binary matrix specifying fixed edges/non-edges");
}


RCPP_MODULE(graph_module)
{
  using namespace Rcpp;

  class_<Base::Graph>("Base")
      .method("sample", &Base::Graph::sample, "Performs multiple sampling steps with optional thinning and burnin period")
      .property("adjacency_matrix", &Base::Graph::adjacency_matrix, "Returns Graph biadjacency matrix")
      .property("fixed", &Base::Graph::fixed, "Returns binary matrix specifying fixed edges/non-edges");

  class_<Weighted::Directed::SG::Graph>("WeightedSGGraph")
      .derives<Base::Graph>("Base")
      .constructor<IntegerMatrix, IntegerMatrix>()
      .const_method("printData", &Weighted::Directed::SG::Graph::printData, "Prints internal data structure to R console")
      .method("sampleStep", &Weighted::Directed::SG::Graph::sampleStep, "Performs a single sampling step");

  class_<Weighted::Directed::DG::Graph>("WeightedDGGraph")
      .derives<Base::Graph>("Base")
      .constructor<IntegerMatrix, IntegerMatrix>()
      .method("sampleStep", &Weighted::Directed::DG::Graph::sampleStep, "Performs a single sampling step");

    class_<Unweighted::Directed::Graph>("UnweightedGraph")
        .derives<Base::Graph>("Base")
        .constructor<IntegerMatrix,IntegerMatrix>()
        .constructor<IntegerVector,IntegerVector,IntegerMatrix>()
        .const_method("printData", &Unweighted::Directed::Graph::printData, "Prints internal data structure to R console")
        .method("sampleStep", &Unweighted::Directed::Graph::sampleStep, "Performs a single sampling step");
}


