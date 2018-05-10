
#include "weighted_graph.h"
#include "AuxiliaryFunctions.h"
using namespace std;
using namespace Rcpp;
using namespace Weighted::Directed;
using namespace Weighted;
using IM = IntegerMatrix;
using IV = IntegerVector;

int sampleDelta(const DeltaRange& dr, default_random_engine& gen) {
  uniform_int_distribution<int> dist(dr.low, dr.up);
  return dist(gen);
}

Graph::Graph(IM adjacency_matrix, IM fixed)
  : ::Directed::Graph<Vertex,Edge>(adjacency_matrix, fixed, false)
{
  auto const nrow = adjacency_matrix.nrow(), ncol = adjacency_matrix.ncol();
  // initialise initial_vertices_
  for (int i = 0; i != nrow + ncol; ++i)
    if (vertices_[i].in_edges.size() != 0)
      initial_vertices_.push_back(&vertices_[i]);
  if(initial_vertices_.size()==0)
    throw invalid_argument("Matrix fully determined by specification");
}

void Graph::sampleKernel(vector<Edge*>& vec) {
    // find a cycle
    Vertex* u0 = sampleFromVector(initial_vertices_, generator_);
    Edge* e = sampleFromVector(u0->in_edges, generator_);
    if (e->visits() == STAR) vec.push_back(e);
    e->increment();
    e = sampleNewFromVector(e->tail_->out_edges, e, generator_);
    if (e->visits() == STAR) vec.push_back(e);
    e->decrement();
    while (e->head_ != u0) {
      if (e->weight()) e = sampleNewFromVector(e->head_->in_edges, e, generator_);
      else e = sampleFromVector(e->head_->in_edges, generator_);
      if (e->visits() == STAR) vec.push_back(e);
      e->increment();
      if (!edges_[e->tail_->index][u0->index - adjacency_matrix_.nrow()].fixed_)
        e = &edges_[e->tail_->index][u0->index - adjacency_matrix_.nrow()];
      else
        e = sampleNewFromVector(e->tail_->out_edges, e, generator_);
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

void Graph::updateAdjacencyMatrix(){

}

void Graph::printData() {
  printMatrix(adjacency_matrix_);
  for (int i = 0; i != vertices_.size(); ++i){
    printVertexData(vertices_[i]);
    Rcout << endl;
  }
  return;
}
