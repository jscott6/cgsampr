
#include <iostream>
#include <interface.h>

using namespace std;
using IM = Rcpp::IntegerMatrix
using IV = Rcpp::IntegerVector

Graph::Graph(IM adjacency_matrix, IM fixed)
  : adjacency_matrix_(clone(adjacency_matrix))
{
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
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator_ = std::default_random_engine(seed);
  StronglyConnectedComponents::Graph sccg(adjacency_matrix_, fixed);
  fixed_ = sccg.fixed_values(fixed);
}

Graph::Graph(IV in_degree, IV out_degree, IM fixed) {
  bool sinkfound = true;
  FeasibleMatrix::Graph fmg(in_degree, out_degree);
  // adjust flow_ until no path is found in residual Graph
  while (sinkfound) {
    sinkfound = fmg.findPath();
    fmg.updateFlow(fmg.calcPathFlow());
  }
  // form matrix from Graph
  Graph(fmg.constructMatrix(r,c), fixed);
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
  Rcout << "This is intended to provide summary information about the graph"
  Rcout << printMatrix(adjacency_matrix_) << endl;
}


Directed::Graph::Directed::Graph(IM adjacency_matrix, IM fixed)
  : Graph(adjacency_matrix, fixed)
{
    int nrow = adjacency_matrix.nrow(), ncol = adjacency_matrix.ncol();
    vertices_ = vector<V>(nrow + ncol);
    for (int i = 0; i != nrow + ncol; ++i) vertices_[i].index = i;
    // allocate memory WITHOUT calling constructor
    edges_ = (E**) malloc(nrow*sizeof(E));
    for (int i = 0; i != nrow; ++i) edges_[i] = (E*) malloc(ncol*sizeof(E));
    // initialise edges
    // applies constructor directly to final address (avoids copy constructor)
    // important to keep correct pointers in Vertex structures
    for (int i = 0; i != nrow; ++i)
      for (int j = 0; j != ncol; ++j)
        new (&edges_[i][j]) E(&vertices_[nrow+j],&vertices_[i],
          fixed(i,j) ,&adjacency_matrix_(i,j));
}


Directed::Graph::Directed::Graph(IV in_degree, IV out_degree, IM fixed)
  : Graph(in_degree, out_degree, fixed),
    Directed::Graph(adjacency_matrix_, fixed_) { }


Undirected::Graph::Undirected::Graph(IM adjacency_matrix, IM fixed)
  :Graph(adjacency_matrix, fixed),
   vertices_(vector<V>(adjacency_matrix.nrow()));
{
  int nrow = adjacency_matrix.nrow();
  for (int i = 0; i != nrow; ++i) vertices_[i].index = i;
  edges_ = (E**) malloc(nrow*sizeof(E));
  for (int i = 0; i != nrow; ++i) edges_[i] = (E*) malloc((nrow-1-i)*sizeof(E));
  for (int i = 0; i != nrow; ++i){
    int k = 0;
    for (int j = (i+1); j != nrow; ++j){
      new (&edges_[i][k]) E(&vertices_[j],&vertices_[i],
        fixed(i,j) ,&adjacency_matrix_(i,j));
      k++;
    }
  }
}


Directed::Graph::Directed::Graph(IV in_degree, IV out_degree, IM fixed):
  : Graph(in_degree, out_degree, fixed),
  Undirected::Graph(adjacency_matrix_, fixed_) { }
