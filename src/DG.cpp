
/*
------------------------------------------------------------
Implementation of Diaconis-Gangolli (1995)
------------------------------------------------------------
*/



#include "graph.h"

using namespace Rcpp;

// completely state-independent sampling procedure
void graph::DG_step(){

  std::uniform_int_distribution<int> row_dist(0, nrow-1);
  std::uniform_int_distribution<int> col_dist(0, ncol-1);

  int i1 = row_dist(generator);
  int i2 = row_dist(generator);
  int j1 = col_dist(generator);
  int j2 = col_dist(generator);

  // ensure we've delineated two rows and columns
  if(i1==i2 || j1 == j2)
    return;
  // return if any delineated element is fixed
  if(fixed(i1,j1)==1 || fixed(i1,j2)==1||fixed(i2,j1)==1||fixed(i2,j2)==1)
    return;

  // only alter the adjacency matrix if one of the following two patterns is observed
  // pattern 1
  if(x(i1,j1)==1 && x(i1,j2)==0 && x(i2,j1)==0 && x(i2,j2)==1){
    x(i1,j1)=0;
    x(i1,j2)=1;
    x(i2,j1)=1;
    x(i2,j2)=0;
  }
  // pattern 2
  if(x(i1,j1)==0 && x(i1,j2)==1 && x(i2,j1)==1 && x(i2,j2)==0){
    x(i1,j1)=1;
    x(i1,j2)=0;
    x(i2,j1)=0;
    x(i2,j2)=1;
  }
  return;
}
