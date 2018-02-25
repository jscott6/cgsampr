
#ifndef GUARD_btable
#define GUARD_btable

#include <Rcpp.h>
#include <vector>
#include <algorithm>
#include <random>

class btable{
private:
  void update_x();
  void init(Rcpp::IntegerMatrix , Rcpp::IntegerMatrix);
  std::vector<int> rowNums, colNums;
  std::vector<std::vector<int> > rows, cols;
  std::discrete_distribution<int> col_dist;
  std::vector<std::uniform_int_distribution<int> > rowSampler, colSampler;
  std::default_random_engine generator;
  int nrow, ncol;
  Rcpp::IntegerMatrix x;
  Rcpp::IntegerMatrix fixed;

public:
  btable(Rcpp::IntegerMatrix , Rcpp::IntegerMatrix );
  btable(Rcpp::IntegerVector , Rcpp::IntegerVector , Rcpp::IntegerMatrix );
  Rcpp::IntegerMatrix get_x();
  Rcpp::IntegerMatrix get_fixed();
  void sample_step();
  Rcpp::List sample_step_show_workings(unsigned int);
  Rcpp::List sample(int, int, int);
  void print_data();
};

#endif
