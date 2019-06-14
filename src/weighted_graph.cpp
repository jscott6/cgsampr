
#include "weighted_graph.h"
#include "AuxiliaryFunctions.h"
using namespace std;
using namespace Rcpp;
using namespace Weighted::Directed::SG;
using namespace Weighted;
using IM = IntegerMatrix;
using IV = IntegerVector;

// samples Delta from its conditional distribution
int sampleDelta(const DeltaRange &dr, const Factor factor, default_random_engine &gen)
{
  double c = dr.up - dr.low - 1 + factor.low + factor.up;
  uniform_real_distribution<float> unif(0.0, 1.0);
  double u = unif(gen);
  if (u <= factor.low/c)
    return dr.low;
  else if (u <= (factor.low + factor.up)/c)
    return dr.up;
  uniform_int_distribution<int> dist(dr.low + 1, dr.up - 1);
  return dist(gen);
}

IM init_fixed(IM adjacency_matrix, IM fixed)
{
  IM fixed0;
  while (true)
  {
    fixed0 = clone(fixed);
  for (int i = 0; i != fixed.nrow(); i++)
    if(sum(adjacency_matrix(i,_)*(1-fixed(i,_))) == 0
       || sum(1-fixed(i,_)) == 1)
      fill(fixed(i,_).begin(), fixed(i,_).end(), 1);
  for (int j = 0; j != fixed.ncol(); j++)
    if(sum(adjacency_matrix(_,j)*(1-fixed(_,j))) == 0
       || sum(1-fixed(_,j)) == 1)
      fill(fixed(_,j).begin(), fixed(_,j).end(), 1);
  if (all(fixed0 == fixed).is_true())
    break;
  }
  return fixed;
}

Graph::Graph(IM adjacency_matrix, IM fixed)
    : ::Directed::Graph<Vertex, Edge>(adjacency_matrix, 
    init_fixed(adjacency_matrix, fixed), false)
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
  Vertex *u0 = sampleFromVector(initial_vertices_, generator_);
  Edge *e = sampleFromVector(u0->in_edges, generator_);
  if (e->visited()) return 1;
  vec.push_back(e);
  e->registerVisit();
  e = sampleNewFromVector(e->tail_->out_edges, e, generator_);
  if (e->visited()) return 1;
  vec.push_back(e);
  e->registerVisit();
  while(e->head_ != u0)
  {
    if (e->weight())
    {
      if(e->head_->in_edges.size() == 1) 
        return 1;
      e = sampleNewFromVector(e->head_->in_edges, e, generator_);
    }
    else
      e = sampleFromVector(e->head_->in_edges, generator_);
    if (e->visited()) return 1;
    vec.push_back(e);
    e->registerVisit();
    if (!edges_[e->tail_->index][u0->index - adjacency_matrix_.nrow()].fixed_)
      e = &edges_[e->tail_->index][u0->index - adjacency_matrix_.nrow()];
    else
      e = sampleNewFromVector(e->tail_->out_edges, e, generator_);
    if (e->visited()) return 1;
    vec.push_back(e);
    e->registerVisit();
  }

  //for (const auto &ed : vec)
  //  printEdgeData(*ed);

  //Rcout << "Length: " << vec.size() << endl;
  
  return 0;
}


DeltaRange Graph::getDeltaRange(vector<Edge *> &vec)
{
  // compute support for Delta
  DeltaRange dr;
  for (int i = 0; i != vec.size(); i++)
    if (i % 2 == 0) dr.low = max(dr.low, -vec[i]->weight());
    else dr.up = min(dr.up, vec[i]->weight());
  return dr;
}


void reset(vector<Edge *> &vec)
{
  for(auto &e : vec) e->reset();
}


void Graph::updateWeights(vector<Edge *> &vec, int delta)
{
  int s = 1;
  for (auto &e : vec)
  {
    e->weight(e->weight() + s*delta);
    s = -s;
  } 
  //reset(vec);
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
  //Rcout << "Delta Range: " << "[" << dr.low << "," << dr.up << "]" << endl;
  Factor factor = getBoundaryWeights(cycle, dr);
  //Rcout << "Factor: " << "[" << factor.low << "," << factor.up << "]" << endl;
  int delta = sampleDelta(dr, factor, generator_);
  //Rcout << "Delta: " << delta << endl;
  if(delta != 0) mixing_ = mixing_ + 1.;
  /*
  if(abs(delta)>1e4)
    {
      for (const auto &ed : cycle)
        printEdgeData(*ed);
      Rcout << endl;
      Rcout << "Delta Range: " << "[" << dr.low << "," << dr.up << "]" << endl;
      Rcout << "Factor: " << "[" << factor.low << "," << factor.up << "]" << endl;
      Rcout << "Delta: " << delta << endl;
      reset(cycle);
      throw invalid_argument("Encountered a Problem");
    }
    */
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


// calculations done in logarithms to avoid overflow
Factor Graph::getBoundaryWeights(const vector<Edge *> &vec, const DeltaRange &dr)
{
  // begin by resetting vertex data
  for(const auto &e : vec)
    e->head_->reset();

  if(dr.up == 0) // compute n_int as are at G_up
  {
    for (int i = 1; i < vec.size(); i +=2) // loop through odd side of cycle
      if (vec[i]->weight() == 0)
        vec[i]->head_->n_int++; 
  }
  else // compute n_up as we are at G_int
  {
    for (int i = 1; i < vec.size(); i +=2) // loop through odd side of cycle
      if (vec[i]->weight() - dr.up == 0)
        vec[i]->head_->n_up--; 
  }

  for (int i = 0; i < vec.size(); i++) 
    if (i % 2 == 0)
    {
      if (vec[i]->weight() + dr.low == 0)
        vec[i]->head_->n_low--; 
    }
    else if (vec[i]->weight() == 0)
      vec[i]->head_->n_low++;


  // compute factors...
  int d1 = vec.front()->tail_->out_edges.size()-1;
  int dk = vec.back()->tail_->out_edges.size()-1;
  double llow = -log(dk) - log(vec.front()->head_->n_low);
  double lup = -log(dk) - log(vec.front()->head_->n_up);
  double lin = log((1./d1 + 1./dk)) - log(vec.front()->head_->n_int);

  //Rcout << "low: " << exp(llow) << endl;
  //Rcout << "up: " << exp(lup) << endl;
  //Rcout << "int: " << exp(lin) << endl;

  for (int i = 1; i != vec.size() - 1; i++)
  {
    if(i % 2 == 0)
    {
      int k = 1*(vec[i]->weight() + dr.low > 0);
      llow = llow - log(vec[i]->head_->n_low - k);
      lin = lin - log(vec[i]->head_->n_int - 1.);
    }
    else
    {
      int k = 1*(vec[i]->weight() - dr.up > 0);
      lup = lup - log(vec[i]->head_->n_up - k);
    }
  }

  //Rcout << endl;
  //Rcout << "low: " << exp(llow) << endl;
  //Rcout << "up: " << exp(lup) << endl;
  //Rcout << "int: " << exp(lin) << endl;

  return Factor(exp(llow - lin), exp(lup - lin));
}


