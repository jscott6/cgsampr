
/*
------------------------------------------------------------
Exposes graph class to R via Rcpp Modules
------------------------------------------------------------
*/



#include "graph.h"

// [[Rcpp::plugins(cpp11)]]

RCPP_MODULE(graph){

  using namespace Rcpp;

  class_<graph>("graph")

  .constructor<IntegerMatrix,IntegerMatrix>()
  .constructor<IntegerVector,IntegerVector,IntegerMatrix>()

  .method("sample", &graph::sample, "Performs MCMC multiple sampling steps as specified by user, return list of tables")
  .method("print_data", &graph::print_data, "Prints internal data structure to R console")
  .method("sample_step_show_workings", &graph::sample_step_show_workings, "Prints each step of a sampling step...")
  .method("print_stub_list", &graph::print_stub_list, "Prints the list of in-stubs and out-stubs for the matching method")
  .method("print_arcList", &graph::print_arcList, "Prints the data of all arcs in the graph")
  .property("x", &graph::get_x, "Returns current state of table")
  .property("fixed", &graph::get_fixed, "Returns matrix tracking which table values are fixed")
  ;
}
