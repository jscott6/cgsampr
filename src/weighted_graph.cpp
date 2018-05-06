
#include "weighted_graph.h"
#include "AuxiliaryFunctions.h"
using namespace std;
using namespace Rcpp;
using namespace Weighted;


int sampleDelta(const DeltaRange& dr, default_random_engine& gen) {
  uniform_int_distribution<int> dist(dr.low, dr.up);
  return dist(gen);
}

Graph::Graph(Rcpp::IntegerMatrix x, Rcpp::IntegerMatrix f) {
  // validate matrices
  if (x.nrow() != f.nrow() || x.ncol() != f.ncol())
    throw invalid_argument("Dimension of x and f do not match");
  bool valid_x = true;
  bool valid_f = true;
  for (int i = 0; i != x.nrow(); ++i) {
    for (int j = 0; j != x.ncol(); ++j) {
      if (x(i,j) < 0) valid_x = false;
      if (f(i,j) != 0 && f(i,j) != 1) valid_f = false;
    }
  }
  if (!valid_x)
    throw invalid_argument("All entries of x must be greater than or equal to zero");
  else if (!valid_f)
    throw invalid_argument("All entries of f must be binary valued");

  adjacency_matrix_ = clone(x);
  int nrow = x.nrow(), ncol = x.ncol();
  // assuming biadjacency matrix
  vertices_ = vector<Vertex>(nrow + ncol);
  // allocate memory WITHOUT calling constructor
  edges_ = (Edge**) malloc(nrow*sizeof(Edge));
  for (int i = 0; i != nrow; ++i) edges_[i] = (Edge*) malloc(ncol*sizeof(Edge));

  // initialise edges
  // applies constructor directly to final address (avoids copy constructor)
  // important to keep correct pointers in Vertex structures
  for (int i = 0; i != nrow; ++i)
    for (int j = 0; j != ncol; ++j)
      new (&edges_[i][j]) Edge(&vertices_[nrow+j],&vertices_[i],
        &adjacency_matrix_(i,j), f(i,j));

  for (int i = 0; i != nrow + ncol; ++i) {
    vertices_[i].index = i;
    if (vertices_[i].in_edges.size() != 0)
      initial_vertices_.push_back(&vertices_[i]);
  }
  if (initial_vertices_.size() == 0)
    throw invalid_argument("Graph fully determined by specification");
  return;
}

void Graph::sampleKernel(vector<Edge*>& vec) {
    // find a cycle
    Vertex* u0 = sampleFromVector(initial_vertices_, generator_);
    Edge* e = sampleFromVector(u0->in_edges, generator_);
    if (e->visits() == STAR) vec.push_back(e);
    e->increment();
    e = sampleNewFromVector(e->tail()->out_edges, e, generator_);
    if (e->visits() == STAR) vec.push_back(e);
    e->decrement();
    while (e->head() != u0) {
      if (e->weight()) e = sampleNewFromVector(e->head()->in_edges, e, generator_);
      else e = sampleFromVector(e->head()->in_edges, generator_);
      if (e->visits() == STAR) vec.push_back(e);
      e->increment();
      if (!edges_[e->tail()->index][u0->index - adjacency_matrix_.nrow()].fixed())
        e = &edges_[e->tail()->index][u0->index - adjacency_matrix_.nrow()];
      else
        e = sampleNewFromVector(e->tail()->out_edges, e, generator_);
      if (e->visits() == STAR) vec.push_back(e);
      e->decrement();
    }
    return;
}

DeltaRange Graph::getDeltaRange(vector<Edge*> & vec) {
  // compute support for Delta
  DeltaRange dr;
  for (const auto& e: vec) {
    if (e->visits() > 0) dr.low = max(dr.low, - e->weight()/e->visits());
    if (e->visits() < 0) dr.up = min(dr.up, - e->weight()/e->visits());
  }
  return dr;
}

void Graph::updateWeights(vector<Edge*>& vec, int delta) {
  for (auto& e: vec) {
    e->weight(e->weight() + e->visits()*delta);
    e->reset();
  }
}

void Graph::sampleStep() {
  vector<Edge*> cycle;
  sampleKernel(cycle);
  DeltaRange dr = getDeltaRange(cycle);
  int delta = sampleDelta(dr,generator_);
  updateWeights(cycle, delta);
  return;
}

void Graph::printData() {
  printMatrix(adjacency_matrix_);
  for (int i = 0; i != vertices_.size(); ++i){
    printVertexData(vertices_[i]);
    Rcout << endl;
  }
  return;
}
