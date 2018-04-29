

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
  .method("printData", &weightedGraph::printData, "Prints internal data structure to R console")
  .method("sampleStep", &weightedGraph::sampleStep, "Performs a single sampling step")
  ;
}
