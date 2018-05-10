
#include <iostream>
#include "interface.h"
#include "StronglyConnectedComponents.h"
#include "FeasibleMatrix.h"
#include "AuxiliaryFunctions.h"

using namespace std;
using namespace Rcpp;
using namespace Base;
using IM = Rcpp::IntegerMatrix;
using IV = Rcpp::IntegerVector;

IM init_adjacency_matrix_(IV in_degree, IV out_degree, IM fixed) {
  bool sinkfound = true;
  FeasibleMatrix::Graph fmg(in_degree, out_degree);
  // adjust flow_ until no path is found in residual Graph
  while (sinkfound) {
    sinkfound = fmg.findPath();
    fmg.updateFlow(fmg.calcPathFlow());
  }
  return fmg.constructMatrix(in_degree,out_degree);
}

IM init_fixed_(IM adjacency_matrix, IM fixed) {
    StronglyConnectedComponents::Graph sccg(adjacency_matrix, fixed);
    return sccg.fixed_values(fixed);
}

void checks(IM adjacency_matrix, IM fixed) {
  if (adjacency_matrix.nrow() != fixed.nrow() ||
      adjacency_matrix.ncol() != fixed.ncol())
    throw invalid_argument("Dimension of x and f do not match");
  bool valid_x = true;
  bool valid_f = true;
  for (int i = 0; i != adjacency_matrix.nrow(); ++i) {
    for (int j = 0; j != adjacency_matrix.ncol(); ++j) {
      if (adjacency_matrix(i,j) < 0) valid_x = false;
      if (fixed(i,j) != 0 && fixed(i,j) != 1) valid_f = false;
    }
  }
  if (!valid_x)
    throw invalid_argument("All entries of x must be greater than or equal to zero");
  else if (!valid_f)
    throw invalid_argument("All entries of f must be binary valued");
}

default_random_engine init_generator_() {
  auto seed = chrono::system_clock::now().time_since_epoch().count();
  return default_random_engine(seed);
}

Graph::Graph(IM adjacency_matrix, IM fixed)
  : adjacency_matrix_(adjacency_matrix),
    fixed_(init_fixed_(adjacency_matrix, fixed)),
    generator_(init_generator_())
{
  checks(adjacency_matrix, fixed);
}

Graph::Graph(IV in_degree, IV out_degree, IM fixed)
  : adjacency_matrix_(init_adjacency_matrix_(in_degree, out_degree, fixed)),
    fixed_(init_fixed_(adjacency_matrix_, fixed)),
    generator_(init_generator_())
{
  checks(adjacency_matrix_, fixed);
}

List Graph::sample(int nsamples, int thin, int burnin) {

  List results(nsamples);
  for (int i = 0; i != nsamples; ++i) {
    for (int j = 0; j != (thin + 1); ++j)
      sampleStep();
    updateAdjacencyMatrix();
    results(i) = clone(adjacency_matrix_);
  }
  return results;
}

void Graph::summary() {
  Rcout << "This is intended to provide summary information about the graph";
  printMatrix(adjacency_matrix_);
}
