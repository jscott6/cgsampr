
#include "ctable_new.h"

// [[Rcpp::plugins(cpp11)]]

RCPP_MODULE(ctable){

  using namespace Rcpp;

  class_<ctable>("ctable")

  .constructor<IntegerMatrix,IntegerMatrix>()

  //.method("print_data", &ctable::print_data, "Prints internal data structure to R console")
  .method("get_cycle", &ctable::get_cycle, "Prints data of a k cycle")
  .property("x", &ctable::get_x, "Returns current state of table")
  .property("fixed", &ctable::get_fixed, "Returns matrix tracking which table values are fixed")
  ;
}
