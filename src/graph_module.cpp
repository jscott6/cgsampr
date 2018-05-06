
/*
------------------------------------------------------------
Exposes Graph class to R via Rcpp Modules
------------------------------------------------------------
*/



#include "graph.h"

// [[Rcpp::plugins(cpp11)]]

RCPP_MODULE(Graph_module){

  using namespace Rcpp;
  using namespace UnWeighted;

  class_<Graph>("Graph")

  .constructor<IntegerMatrix,IntegerMatrix,bool>()
  .constructor<IntegerVector,IntegerVector,IntegerMatrix,bool>()

  .method("sample",
          &Graph::sample,
          "Performs multiple sampling steps as specified by user, return list of tables")
  .method("printData",
          &Graph::printData,
          "Prints internal data structure to R console")
  .method("printStubList",
          &Graph::printStubList,
          "Prints the list of in-stubs and out-stubs for the matching method")
  .method("printEdgeList",
          &Graph::printEdgeList,
          "Prints the data of all arcs in the Graph")
  .property("x",
            &Graph::adjacency_matrix,
            "Returns current state of table")
  .property("fixed",
            &Graph::fixed,
            "Returns matrix tracking_ which table values are fixed_")
  ;
}
