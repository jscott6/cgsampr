

/*
#ifndef GUARD_ctable
#define GUARD_ctable


#include <Rcpp.h>
#include <vector>
#include <random>
#include <algorithm>




class entry{
  friend class ctable;
  unsigned int id, row, col row_loc, col_loc;
  unsigned int col;
  std::array<unsigned int, 2> constraints_loc
public:
  entry(unsigned int id, unsigned int i=0, unsigned int j=0):
    row(i),
    col(j){};

}

// given a vector, will add the id of the entry to the end
// entry class must store its location in the vector...
// are we storing for first constraint or second?
void add_entry(std::vector<unsigned int>& vec, &entry e){
  e.row_loc = vec.size();
  vec.push_back(e.id);
}


class ctable{
private:
  unsigned int nrow, ncol, kmax;
  Rcpp::IntegerMatrix x;
  Rcpp::IntegerMatrix fixed;
  std::vector<std::vector<unsigned int> > rows, cols;
  std::vector<unsigned int> rowNums, colNums, c;
  std::vector<entry> oddEntries;
  std::vector<entry>::iterator end;
  std::vector<std::vector<std::vector<unsigned int> > > tracking;
  std::default_random_engine generator;
  std::uniform_int_distribution<unsigned int> colDist, lenDist;

public:
  ctable(Rcpp::IntegerMatrix, Rcpp::IntegerMatrix);
  void print_data();
  void get_cycle();
  unsigned int sample_row(const unsigned int&, const unsigned int&);
  unsigned int sample_col(const unsigned int&, const unsigned int&);
  Rcpp::IntegerMatrix get_x(){return x;};
  Rcpp::IntegerMatrix get_fixed(){return fixed;};
};


#endif
*/
