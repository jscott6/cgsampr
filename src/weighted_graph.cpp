
#include "weighted_graph.h"
#include "AuxiliaryFunctions.h"
using namespace std;
using namespace Rcpp;
using namespace Weighted::Directed::SG;
using namespace Weighted;
using IM = IntegerMatrix;
using IV = IntegerVector;

void getVerticesCycleData(const vector<Edge *> &vec, const DeltaRange& dr);
void updateFactor(Vertex *v, float x, Factor& factor);
double power(float x, uint n);

// samples Delta from its conditional distribution
int sampleDelta(const DeltaRange &dr, const Factor factor, default_random_engine &gen)
{
  if(dr.up==dr.low) return 0;
  float c = dr.up - dr.low - 1 + factor.low + factor.up;
  uniform_real_distribution<float> unif(0.0, 1.0);
  float u = unif(gen);
  if (u <= factor.low/c)
    return dr.low;
  else if (u <= (factor.low + factor.up)/c)
    return dr.up;
  uniform_int_distribution<int> dist(dr.low + 1, dr.up - 1);
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
  vector<Edge *> longvec;
  Vertex *u0 = sampleFromVector(initial_vertices_, generator_);
  Edge *e = sampleFromVector(u0->in_edges, generator_);
  longvec.push_back(e);
  if (e->even() == STAR && e->odd() == STAR)
    vec.push_back(e);
  e->incrementEven();
  e = sampleNewFromVector(e->tail_->out_edges, e, generator_);
  longvec.push_back(e);
  if (e->even() == STAR && e->odd() == STAR)
    vec.push_back(e);
  e->incrementOdd();
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
    longvec.push_back(e);
    if (e->even() == STAR && e->odd() == STAR)
      vec.push_back(e);
    e->incrementEven();
    if (!edges_[e->tail_->index][u0->index - adjacency_matrix_.nrow()].fixed_)
      e = &edges_[e->tail_->index][u0->index - adjacency_matrix_.nrow()];
    else
      e = sampleNewFromVector(e->tail_->out_edges, e, generator_);
    longvec.push_back(e);
    if (e->even() == STAR && e->odd() == STAR)
      vec.push_back(e);
    e->incrementOdd();

    for (const auto &ed : longvec)
      printEdgeData(*ed);

    Rcout << "Length: " << longvec.size() << endl;
  }
  return 0;
}

// int Graph::sampleKernel(vector<Edge *> &vec)
// {
//   // find a cycle
//   Vertex *u0 = sampleFromVector(initial_vertices_, generator_);
//   Edge *e = sampleFromVector(u0->in_edges, generator_);
//   if (e->even() == STAR && e->odd() == STAR)
//     vec.push_back(e);
//   e->incrementEven();
//   e = sampleNewFromVector(e->tail_->out_edges, e, generator_);
//   if (e->even() == STAR && e->odd() == STAR)
//     vec.push_back(e);
//   e->incrementOdd();
//   while (e->head_ != u0)
//   {
//     if (e->weight())
//     {
//       if(e->head_->in_edges.size() == 1) 
//         return 1;
//       e = sampleNewFromVector(e->head_->in_edges, e, generator_);
//     }
//     else
//       e = sampleFromVector(e->head_->in_edges, generator_);
//     if (e->even() == STAR && e->odd() == STAR)
//       vec.push_back(e);
//     e->incrementEven();
//     if (!edges_[e->tail_->index][u0->index - adjacency_matrix_.nrow()].fixed_)
//       e = &edges_[e->tail_->index][u0->index - adjacency_matrix_.nrow()];
//     else
//       e = sampleNewFromVector(e->tail_->out_edges, e, generator_);
//     if (e->even() == STAR && e->odd() == STAR)
//       vec.push_back(e);
//     e->incrementOdd();
//   }
//   return 0;
// }

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
  for (auto &e : vec){
    e->weight(e->weight() + (e->even() - e->odd())* delta);
  }
    
  reset(vec);
}



void Graph::sampleStep()
{
  // reserve space in here to avoid reallocations!!
  // we are storing unique edges, so maximum size cannot
  // exceed nrow*ncol
  vector<Edge *> cycle;
  int discard = sampleKernel(cycle);
  if (discard){
    reset(cycle);
    return;
  }
  DeltaRange dr = getDeltaRange(cycle);
  Rcout << "Delta Range: " << "[" << dr.low << "," << dr.up << "]" << endl;
  Factor factor = getBoundaryWeights(cycle, dr);
  Rcout << "Factor: " << "[" << factor.low << "," << factor.up << "]" << endl;
  int delta = sampleDelta(dr, factor, generator_);
  Rcout << "Delta: " << delta << endl;
  updateWeights(cycle, delta);
  reset(cycle);
}



void Graph::updateAdjacencyMatrix()
{
}

void Graph::printData() const
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


Factor Graph::getBoundaryWeights(const vector<Edge *> &vec, const DeltaRange& dr)
{
  // ensure correct data populated before calculation
  getVerticesCycleData(vec, dr);
  Factor factor;

  // adjust for out edges of first and last vertices
  double sum = vec.front()->tail_->out_edges.size() + vec.back()->tail_->out_edges.size() - 2;
  factor.up *= (vec.back()->tail_->out_edges.size()-1)/sum;
  factor.low *= (vec.front()->tail_->out_edges.size()-1)/sum;

  // first vertex treated separately
  float x = (float) (vec.front()->head_->in_edges.size() + ((dr.up == 0) ? vec.front()->head_->cycle_data.up_zeros : 0));
  factor.up *= x/(x - vec.front()->head_->cycle_data.up_zeros );
  factor.low *= x/(x - vec.front()->head_->cycle_data.low_zeros);
  vec.front()->head_->cycle_data.reset();
  for (int i = 1 ; i != vec.size(); i++)
  {
    updateFactor(vec[i]->head_,
                 vec[i]->head_->in_edges.size() 
                  + ((dr.up == 0) ? vec[i]->head_->cycle_data.up_zeros : 0) - 1,
                factor);
  }
  return factor;
}


void getVerticesCycleData(const vector<Edge *> &vec, const DeltaRange& dr)
{
  for (const auto &e : vec)
  {
    e->head_->cycle_data.visits += e->even();
    if (!(e->weight() + (e->even() - e->odd()) * dr.up))
    {
      e->head_->cycle_data.up_zero_visits += e->odd();
      e->head_->cycle_data.up_zeros++;
    }
    if (!(e->weight() + (e->even() - e->odd()) * dr.low))
    {
      e->head_->cycle_data.low_zero_visits += e->even();
      e->head_->cycle_data.low_zeros++;
    }
  }
}

void updateFactor(Vertex *v, float x, Factor& factor)
{
  if (v->cycle_data.visits)
  {
    factor.up *= power(x / (x - v->cycle_data.up_zeros+1), v->cycle_data.up_zero_visits);
    factor.up *= power(x / (x - v->cycle_data.up_zeros), v->cycle_data.visits - v->cycle_data.up_zero_visits);
    factor.low *= power(x / (x - v->cycle_data.low_zeros+1), v->cycle_data.low_zero_visits);
    factor.low *= power(x / (x - v->cycle_data.low_zeros), v->cycle_data.visits - v->cycle_data.low_zero_visits);
    v->cycle_data.reset();
  }
}