
#include <iostream>
#include <interface.h>

using namespace std;
using IM = Rcpp::IntegerMatrix
using IV = Rcpp::IntegerVector

AbstractGraph::AbstractGraph(IM adjacency_matrix, IM fixed, bool digraph=true)
  : adjacency_matrix_(adjacency_matrix),
    digraph_(digraph)
{
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator_ = std::default_random_engine(seed);
  StronglyConnectedComponents::Graph G(adjacency_matrix_, fixed);
  fixed_ = G.fixed_values(fixed);
}

AbstractGraph::AbstractGraph(IV in_degree, IV out_degree, bool digraph=true)
  : digraph_(digraph)
{
  bool sinkfound = true;
  FeasibleMatrix::Graph G(in_degree, out_degree);
  // adjust flow_ until no path is found in residual Graph
  while (sinkfound) {
    sinkfound = G.findPath();
    G.updateFlow(G.calcPathFlow());
  }
  // form matrix from Graph
  adjacency_matrix_ = G.constructMatrix(r,c);
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator_ = std::default_random_engine(seed);
  StronglyConnectedComponents::Graph G(adjacency_matrix_, fixed);
  fixed_ = G.fixed_values(fixed);
}

List AbstractGraph::sample(int nsamples, int thin, int burnin) {

  List results(nsamples);

  for (int i = 0; i != nsamples; ++i) {
    for (int j = 0; j != (thin + 1); ++j)
      sampleStep();
    updateAdjacencyMatrix();
    results(i) = clone(adjacency_matrix_);
  }
  return results;
}


void AbstractGraph::summary() {
  Rcout << "This is intended to provide summary information about the graph"
  Rcout << printMatrix(adjacency_matrix_) << endl;
}
