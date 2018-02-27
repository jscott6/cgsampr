
#ifndef GUARD_graph
#define GUARD_graph

#include <Rcpp.h>
#include <vector>
#include <algorithm>
#include <random>
#include <string>


struct arc{
  int head;
  int tail;
};

class graph{
private:
  void update_x();
  void init(Rcpp::IntegerMatrix , Rcpp::IntegerMatrix);
  void switch_step();
  void DG_step();
  void SG_step();
  void matching_step();
  void update_datastructures();
  std::vector<int> oneNums, zeroNums, inStubs, outStubs;
  std::vector<arc> arcList;
  std::vector<std::vector<int> > ones, zeros;
  std::discrete_distribution<int> one_dist;
  std::vector<std::uniform_int_distribution<int> > oneSampler, zeroSampler;
  std::default_random_engine generator;
  int nrow, ncol, nStubs;
  Rcpp::IntegerMatrix x;
  Rcpp::IntegerMatrix fixed;

public:
  graph(Rcpp::IntegerMatrix , Rcpp::IntegerMatrix );
  graph(Rcpp::IntegerVector , Rcpp::IntegerVector , Rcpp::IntegerMatrix );
  Rcpp::IntegerMatrix get_x();
  Rcpp::IntegerMatrix get_fixed();
  Rcpp::List sample_step_show_workings(unsigned int);
  Rcpp::List sample(std::string method, int, int, int);
  void print_data();
  void print_arcList();
  void print_stub_list();
};

#endif
