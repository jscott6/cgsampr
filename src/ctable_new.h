


#ifndef GUARD_ctable_new
#define GUARD_ctable_new

#include <Rcpp.h>
#include <vector>
#include <random>
#include <algorithm>
#include <queue>

enum boundtype{up,low};

class entry;

class constraint{
  std::vector<entry*> entries;
  std::default_random_engine generator;
public:
    constraint();
    void add_entry(entry*);
    void remove_entry(unsigned int);
    unsigned int size();
    unsigned int get_last();
    entry* sample(entry*&);
    entry* sample();
};

class entry{
  unsigned int row, col, row_loc, col_loc;
  int multiplier;
  bool visited;
  int* valuep;
  constraint* const row_constraint, * const col_constraint;
public:
  entry(unsigned int, unsigned int, constraint*, constraint*, int*);
  void reset();
  void increment_multiplier(){++multiplier;};
  void decrement_multiplier(){--multiplier;};
  void set_visited(bool b){visited=b;};
  void set_value(int);
  void set_row_loc(unsigned int i){row_loc=i;};
  void set_col_loc(unsigned int j){col_loc=j;};
  void update_bound(unsigned int&, boundtype);
  int get_multiplier(){return multiplier;};
  int get_value();
  unsigned int get_visited(){return visited;};
  unsigned int get_row(){return row;};
  unsigned int get_col(){return col;};
  constraint* const get_row_constraint(){return row_constraint;};
  constraint* const get_col_constraint(){return col_constraint;};
};


class ctable{
private:
  unsigned int nrow, ncol, kmax, lowbound, upbound, table_total;
  float lowfactor, upfactor;
  Rcpp::IntegerMatrix x, fixed;
  std::vector<unsigned int> c;
  std::vector<constraint> constraints;
  std::vector<std::vector<entry> > entries;
  std::vector<entry*> cycle;
  std::vector<entry*>::iterator end;
  std::default_random_engine generator;
  std::queue<entry*> first_col;
public:
  ctable(Rcpp::IntegerMatrix, Rcpp::IntegerMatrix);
  void add_row(constraint&, entry*);
  void add_col(constraint&, entry*);
  void get_cycle();
  void calc_bounds();
  void calc_factor();
  void update_cycle(std::vector<entry*>::iterator&, entry*&);
  void reset_cycle();
  Rcpp::IntegerMatrix get_x(){return x;};
  Rcpp::IntegerMatrix get_fixed(){return fixed;};
};


#endif
