

#ifndef GUARD_graph_switch
#define GUARD_graph_switch

#include <Rcpp.h>
#include <vector>
#include <algorithm>
#include <random>


struct arc{
  int head;
  int tail;
};

class graph_switch{
private:
  void init(Rcpp::IntegerMatrix , Rcpp::IntegerMatrix);
  std::default_random_engine generator;
  Rcpp::IntegerMatrix x;
  std::vector<arc> arc_list;
  Rcpp::IntegerMatrix fixed;

public:
  graph_switch(Rcpp::IntegerMatrix , Rcpp::IntegerMatrix );
  Rcpp::IntegerMatrix get_x();
  Rcpp::IntegerMatrix get_fixed();
  void sample_step();
  void print_arc_list();
};

#endif
