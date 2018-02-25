
#include "btable.h"

// [[Rcpp::plugins(cpp11)]]

RCPP_MODULE(btable){

  using namespace Rcpp;

  class_<btable>("btable")

  .constructor<IntegerMatrix,IntegerMatrix>()
  .constructor<IntegerVector,IntegerVector,IntegerMatrix>()

  .method("sample_step", &btable::sample_step, "Performs a single MCMC sampling step")
  .method("sample", &btable::sample, "Performs MCMC multiple sampling steps as specified by user, return list of tables")
  .method("print_data", &btable::print_data, "Prints internal data structure to R console")
  .method("sample_step_show_workings", &btable::sample_step_show_workings, "Prints each step of a sampling step...")
  .property("x", &btable::get_x, "Returns current state of table")
  .property("fixed", &btable::get_fixed, "Returns matrix tracking which table values are fixed")
  ;
}
