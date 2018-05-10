
#include <iostream>
#include "interface.h"
#include "StronglyConnectedComponents.h"
#include "FeasibleMatrix.h"
#include "AuxiliaryFunctions.h"

using namespace std;
using namespace Rcpp;
using namespace Base;
using IM = IntegerMatrix;
using IV = IntegerVector;

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

 void checkGraphDetermined(IM fixed) {
  bool undetermined = true;
  for (int i = 0; i != fixed.nrow(); ++i){
    for (int j = 0; j != fixed.ncol(); ++j){
      if (fixed(i,j)==0) undetermined = false;
    }
  }
  if(!undetermined)
    throw invalid_argument("Matrix fully determined by specification");
}

void checkDegrees(IV in_degree, IV out_degree, IM fixed) {
  if(in_degree.size() != fixed.nrow() || out_degree.size() != fixed.ncol())
    throw invalid_argument("Degree sequences and fixed matrix of incompatible dimensions");
  for(int i = 0; i != in_degree.size(); ++i)
    if(in_degree(i)<0)
      throw invalid_argument("in-degree sequence must be positive");
  for(int i = 0; i != out_degree.size(); ++i)
    if(out_degree(i)<0)
      throw invalid_argument("out-degree sequence must be positive");
}


IM initFixed(IM adjacency_matrix, IM fixed, bool search) {
    checks(adjacency_matrix, fixed);
    if(!search) return fixed;
    StronglyConnectedComponents::Graph sccg(adjacency_matrix, fixed);
    fixed = sccg.fixed_values(fixed);
    checkGraphDetermined(fixed);
    return fixed;
}

IM initAdjacencyMatrix(IV in_degree, IV out_degree, IM fixed) {
  checkDegrees(in_degree, out_degree, fixed);
  bool sinkfound = true;
  FeasibleMatrix::Graph fmg(in_degree, out_degree);
  // adjust flow_ until no path is found in residual Graph
  while (sinkfound) {
    sinkfound = fmg.findPath();
    fmg.updateFlow(fmg.calcPathFlow());
  }
  return fmg.constructMatrix(in_degree,out_degree);
}

default_random_engine initGenerator() {
  auto seed = chrono::system_clock::now().time_since_epoch().count();
  return default_random_engine(seed);
}


Graph::Graph(IM adjacency_matrix, IM fixed, bool search)
  : adjacency_matrix_(adjacency_matrix),
    fixed_(initFixed(adjacency_matrix, fixed, search)),
    generator_(initGenerator()) { }

Graph::Graph(IV in_degree, IV out_degree, IM fixed ,bool search)
  : adjacency_matrix_(initAdjacencyMatrix(in_degree, out_degree, fixed)),
    fixed_(initFixed(adjacency_matrix_, fixed, search)),
    generator_(initGenerator())
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

void Graph::sampleStep() {

}

void Graph::updateAdjacencyMatrix() {

}
