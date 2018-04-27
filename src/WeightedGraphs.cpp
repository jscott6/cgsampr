
#include "weightedGraphs.h"


weightedGraph::weightedGraph(Rcpp::IntegerMatrix x0, Rcpp::IntegerMatrix f){

  adj_matrix = clone(x0);
  int nrow = x0.nrow(), ncol = x0.ncol();

  // assuming biadjacency matrix
  vertices = vector<vertex>(nrow + ncol);
  edges = vector<vector<edge> >(nrow, vector<edge>(ncol));

  // initialise edges
  for(int i=0;i!=nrow;++i){
    for(int j=0;j!=ncol;++j){
      edge e(vertices[nrow+j],vertices[i], adj_matrix[i][j]);
      edges[i][j] = e;
    }
  }
  return;
}
