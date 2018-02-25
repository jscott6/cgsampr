
#include "graph.h"

// [[Rcpp::plugins(cpp11)]]

RCPP_MODULE(graph){

  using namespace Rcpp;

  class_<graph>("graph")

  .constructor<IntegerMatrix,IntegerMatrix>()
  .constructor<IntegerVector,IntegerVector,IntegerMatrix>()

  .method("sample_step", &graph::sample_step, "Performs a single MCMC sampling step")
  .method("sample", &graph::sample, "Performs MCMC multiple sampling steps as specified by user, return list of tables")
  .method("print_data", &graph::print_data, "Prints internal data structure to R console")
  .method("sample_step_show_workings", &graph::sample_step_show_workings, "Prints each step of a sampling step...")
  .property("x", &graph::get_x, "Returns current state of table")
  .property("fixed", &graph::get_fixed, "Returns matrix tracking which table values are fixed")
  ;
}
