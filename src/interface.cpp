
#include <iostream>
#include "interface.h"
#include "StronglyConnectedComponents.h"
#include "AuxiliaryFunctions.h"

using namespace std;
using namespace Rcpp;
using namespace Base;
using IM = IntegerMatrix;
using IV = IntegerVector;

void checks(IM adjacency_matrix, IM fixed) {
  if (adjacency_matrix.nrow() != fixed.nrow() ||
      adjacency_matrix.ncol() != fixed.ncol())
    throw invalid_argument("Dimension of adjacency matrix and fixed matrix do not match");
  if(min(adjacency_matrix)<0) 
    throw invalid_argument("All entries of adjacency matrix must be non-negative");
  if(min(fixed) < 0 || max(fixed) > 1)
    throw invalid_argument("All entries of fixed matrix must be binary valued");
  if(min(fixed) > 0)
    throw invalid_argument("Graph fully determined by specification");
}

 
// [[Rcpp::export]]
IM initFixed(IM adjacency_matrix, IM fixed, bool search) {
    checks(adjacency_matrix, fixed);
    if(!search) return fixed;
    StronglyConnectedComponents::Graph sccg(adjacency_matrix, fixed);
    return sccg.fixed_values(fixed);
}

default_random_engine initGenerator() {
  auto seed = chrono::system_clock::now().time_since_epoch().count();
  return default_random_engine(seed);
}

Graph::Graph(IM adjacency_matrix, IM fixed, bool search)
  : adjacency_matrix_(clone(adjacency_matrix)),
    fixed_(initFixed(adjacency_matrix, fixed, search)),
    generator_(initGenerator()),
    mixing_(0.) 
    { checks(adjacency_matrix_, fixed_); }

List Graph::sample(int nsamples, int thin, int burnin) {

  List results(nsamples);
  mixing_ = 0.;
  for (int i = 0; i != nsamples; ++i) {
    for (int j = 0; j != (thin + 1); ++j)
      sampleStep();
    updateAdjacencyMatrix();
    results(i) = clone(adjacency_matrix_);
  }
  mixing_ = mixing_ / (double) (nsamples*thin);
  return results;
}

void Graph::summary() {
  Rcout << "This is intended to provide summary information about the graph";
  printMatrix(adjacency_matrix_);
}

void Graph::sampleStep() {

}

void Graph::updateAdjacencyMatrix() {

}
