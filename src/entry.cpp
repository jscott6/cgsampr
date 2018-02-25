
#include "ctable_new.h"

using namespace Rcpp;
using namespace std;


entry::entry(unsigned int r, unsigned int c, constraint* rowcp, constraint* colcp, int* valp)
  : row(r),
    col(c),
    row_constraint(rowcp),
    col_constraint(colcp),
    row_loc(0),
    col_loc(0),
    multiplier(0),
    visited(false),
    valuep(valp){
}


void entry::set_value(int i){
  *valuep = i;
}

int entry::get_value(){
  return *valuep;
}

void entry::reset(){
  multiplier=0;
  visited=false;
}

void entry::update_bound(unsigned int& bound, boundtype t){

  if(t==up && multiplier>=0) return;
  if(t==low && multiplier<=0) return;
  bound = min(bound, (unsigned)((*valuep)/abs(multiplier)));
  return;
}
