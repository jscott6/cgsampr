

/*
------------------------------------------------------------
Exposes weightedGraph class to R via Rcpp Modules
------------------------------------------------------------
*/



#include "weightedGraphs.h"

// [[Rcpp::plugins(cpp11)]]

RCPP_MODULE(weightedGraph){

  using namespace Rcpp;

  class_<weightedGraph>("weightedGraph")

  .constructor<IntegerMatrix,IntegerMatrix>()
  .method("print_data", &weightedGraph::print_data, "Prints internal data structure to R console")
  ;
}
