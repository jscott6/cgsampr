
//
// ------------------------------------------------------------
// Implementation of Diaconis-Gangolli (1995)
// ------------------------------------------------------------
//
#include "graph.h"
using namespace Rcpp;
using namespace UnWeighted;

void Graph::DGStep() {
  std::uniform_int_distribution<int> row_dist(0, nrow_ - 1);
  std::uniform_int_distribution<int> col_dist(0, ncol_ - 1);

  int i1 = row_dist(generator_);
  int i2 = row_dist(generator_);
  int j1 = col_dist(generator_);
  int j2 = col_dist(generator_);

  // ensure we've delineated two rows and columns
  if (i1 == i2 || j1 == j2)
    return;
  // return if any delineated element is fixed_
  if (fixed_(i1,j1) == 1 || fixed_(i1,j2) == 1 ||
      fixed_(i2,j1) == 1 || fixed_(i2,j2) == 1)
    return;
  // only alter the adjacency matrix if one of the following two patterns is observed
  // pattern 1
  if (adjacency_matrix_(i1,j1) == 1 && adjacency_matrix_(i1,j2) == 0 && adjacency_matrix_(i2,j1) == 0 && adjacency_matrix_(i2,j2) == 1) {
    adjacency_matrix_(i1,j1) = 0;
    adjacency_matrix_(i1,j2) = 1;
    adjacency_matrix_(i2,j1) = 1;
    adjacency_matrix_(i2,j2) = 0;
  }
  else if (adjacency_matrix_(i1,j1) == 0 && adjacency_matrix_(i1,j2) == 1 && adjacency_matrix_(i2,j1) == 1 && adjacency_matrix_(i2,j2) == 0){
    adjacency_matrix_(i1,j1) = 1;
    adjacency_matrix_(i1,j2) = 0;
    adjacency_matrix_(i2,j1) = 0;
    adjacency_matrix_(i2,j2) = 1;
  }
  return;
}
