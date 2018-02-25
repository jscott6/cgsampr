

#include "graph_switch.h"

// [[Rcpp::plugins(cpp11)]]

RCPP_MODULE(graph_switch){

  using namespace Rcpp;

  class_<graph>("graph_switch")

  .constructor<IntegerMatrix,IntegerMatrix>()

  .method("sample_step", &graph_switch::sample_step, "Performs a single MCMC sampling step")
  //.method("sample", &graph_switch::sample, "Performs MCMC multiple sampling steps as specified by user, return list of tables")
  //.method("print_data", &graph_switch::print_data, "Prints internal data structure to R console")
  //.method("sample_step_show_workings", &graph::sample_step_show_workings, "Prints each step of a sampling step...")
  .property("x", &graph_switch::get_x, "Returns current state of table")
  .property("fixed", &graph_switch::get_fixed, "Returns matrix tracking which table values are fixed")
  ;
}
