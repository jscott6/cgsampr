
#include "weighted_graph.h"
#include "AuxiliaryFunctions.h"
using namespace std;
using namespace Rcpp;
using namespace Weighted::Directed;
using namespace Weighted;
using IM = IntegerMatrix;
using IV = IntegerVector;

int sampleDelta(const DeltaRange &dr, default_random_engine &gen)
{
  uniform_int_distribution<int> dist(dr.low, dr.up);
  return dist(gen);
}

void fixRow(int i, IM &fixed)
{
  for (int j = 0; j != fixed.ncol(); ++j)
    fixed(i, j) = 1;
  return;
}

void fixCol(int j, IM &fixed)
{
  for (int i = 0; i != fixed.nrow(); ++i)
    fixed(i, j) = 1;
  return;
}

IM init_fixed(IM adjacency_matrix, IM fixed)
{
  const auto nrow = fixed.nrow(), ncol = fixed.ncol();
  for (int i = 0; i != nrow; ++i)
  {
    int sum = 0;
    for (int j = 0; j != ncol; ++j)
    {
      if(!fixed(i,j))
        sum += adjacency_matrix(i,j);
    }
    if(!sum)
      fixRow(i, fixed);
  }
    for (int j = 0; j != ncol; ++j)
  {
    int sum = 0;
    for (int i = 0; i != nrow; ++i)
    {
      if(!fixed(i,j))
        sum += adjacency_matrix(i,j);
    }
    if(!sum)
      fixCol(j, fixed);
  }
  return fixed;
}

Graph::Graph(IM adjacency_matrix, IM fixed)
    : ::Directed::Graph<Vertex, Edge>(adjacency_matrix, init_fixed(adjacency_matrix, fixed), false)
{
  auto const nrow = adjacency_matrix.nrow(), ncol = adjacency_matrix.ncol();
  // initialise initial_vertices_
  for (int i = 0; i != nrow + ncol; ++i)
    if (vertices_[i].in_edges.size() != 0)
      initial_vertices_.push_back(&vertices_[i]);
  if (initial_vertices_.size() == 0)
    throw invalid_argument("Matrix fully determined by specification");
}

int Graph::sampleKernel(vector<Edge *> &vec)
{
  // find a cycle
  Vertex *u0 = sampleFromVector(initial_vertices_, generator_);
  Edge *e = sampleFromVector(u0->in_edges, generator_);
  if (e->even() == STAR && e->odd() == STAR)
    vec.push_back(e);
  e->increment();
  e = sampleNewFromVector(e->tail_->out_edges, e, generator_);
  if (e->even() == STAR && e->odd() == STAR)
    vec.push_back(e);
  e->decrement();
  while (e->head_ != u0)
  {
    if (e->weight())
    {
      if(e->head_->in_edges.size() == 1) 
        return 1;
      e = sampleNewFromVector(e->head_->in_edges, e, generator_);
    }
    else
      e = sampleFromVector(e->head_->in_edges, generator_);
    if (e->even() == STAR && e->odd() == STAR)
      vec.push_back(e);
    e->increment();
    if (!edges_[e->tail_->index][u0->index - adjacency_matrix_.nrow()].fixed_)
      e = &edges_[e->tail_->index][u0->index - adjacency_matrix_.nrow()];
    else
      e = sampleNewFromVector(e->tail_->out_edges, e, generator_);
    if (e->even() == STAR && e->odd() == STAR)
      vec.push_back(e);
    e->decrement();
  }
  return 0;
}

DeltaRange Graph::getDeltaRange(vector<Edge *> &vec)
{
  // compute support for Delta
  DeltaRange dr;
  for (const auto &e : vec)
  {
    if ((e->even() - e->odd()) > 0)
      dr.low = max(dr.low, -e->weight() / (e->even() - e->odd()));
    if ((e->even() - e->odd()) < 0)
      dr.up = min(dr.up, -e->weight() / (e->even() - e->odd()));
  }
  return dr;
}

void reset(vector<Edge *> &vec)
{
  for(auto &e : vec)
    e->reset();
}


void Graph::updateWeights(vector<Edge *> &vec, int delta)
{
  for (auto &e : vec)
    e->weight(e->weight() + (e->even() - e->odd())* delta);
  reset(vec);
}


void Graph::sampleStep()
{
  // reserve space in here to avoid reallocations!!
  // we are storing unique edges, so maximum size cannot
  // exceed nrow*ncol
  vector<Edge *> cycle;
  if (sampleKernel(cycle)){
    reset(cycle);
    return;
  }
  DeltaRange dr = getDeltaRange(cycle);
  int delta = sampleDelta(dr, generator_);
  updateWeights(cycle, delta);
}

void Graph::updateAdjacencyMatrix()
{
}

void Graph::printData()
{
  printMatrix(adjacency_matrix_);
  for (int i = 0; i != vertices_.size(); ++i)
  {
    printVertexData(vertices_[i]);
    Rcout << endl;
  }
}


double power(float x, uint n)
{
  double r = 1.0;
  while (n > 0)
  {
    r *= x;
    --n;
  }
  return r;
}


void updateFactor(Vertex *v, int x, double& factor)
{
  if (!v->even)
  {
    factor *= power((float) x / (float) (x - v->zeros+1), v->even_zeros);
    factor *= power((float) x / (float) (x - v->zeros), v->even - v->even_zeros);
    v->reset();
  }
}



void Graph::getBoundaryWeights(vector<Edge *> &vec) 
{
  // store number of even side visits to each vertex
  for (const auto& e: vec)
  {
    e->head_->even += e->even();
  }
}
