
#include "weightedGraph.h"
#include "AuxiliaryFunctions.h"

using namespace std;
using namespace Rcpp;

/*
generic function to sample from a vector
given some vector, return a random (uniform) element from it
Assumes vector is non-empty
*/
template<class T>
T sampleFromVector(const vector<T> &vec, default_random_engine& gen){
  uniform_int_distribution<int> dist(0, vec.size()-1);
  return vec[dist(gen)];
}

/*
generic function to sample NEW element from a vector
given some vector, return a new random (uniform) element from it
Assumes vector is of size 2 or more
*/
template<class T>
T sampleNewFromVector(const vector<T> &vec, T x, default_random_engine& gen){
  uniform_int_distribution<int> dist(0, vec.size()-2);
  T res = vec[dist(gen)];
  if(res==x)
    res = vec.back();
  return res;
}


int sampleDelta(const deltaRange& dr, default_random_engine& generator){
  uniform_int_distribution<int> dist(dr.low, dr.up);
  return dist(generator);
}

weightedGraph::weightedGraph(Rcpp::IntegerMatrix x0, Rcpp::IntegerMatrix f){

  // validate matrices
  if(x0.nrow()!=f.nrow() || x0.ncol()!=f.ncol())
    throw invalid_argument("Dimension of x and f do not match");

  bool validx = true;
  bool validf = true;

  for(int i=0;i!=x0.nrow();++i){
    for(int j=0;j!=x0.ncol();++j){
      if(x0(i,j)<0) validx = false;
      if(f(i,j)!=0 && f(i,j)!=1) validf = false;
    }
  }

  if(!validx)
    throw invalid_argument("All entries of x must be greater than or equal to zero");
  else if(!validf)
    throw invalid_argument("All entries of f must be binary valued");

  adj_matrix = clone(x0);
  int nrow = x0.nrow(), ncol = x0.ncol();

  // assuming biadjacency matrix
  vertices = vector<vertex>(nrow + ncol);

  // allocate memory WITHOUT calling constructor
  edges = (edge**)malloc(nrow*sizeof(edge));
  for(int i=0; i!=nrow; ++i) edges[i] = (edge*)malloc(ncol*sizeof(edge));

  // initialise edges
  for(int i=0;i!=nrow;++i)
    for(int j=0;j!=ncol;++j)
      // applies constructor directly to final address (avoids copy constructor)
      // important to keep correct pointers in vertex structures
      new (&edges[i][j]) edge(&vertices[nrow+j],&vertices[i], &adj_matrix(i,j), f(i,j));

  for(int i=0; i!=nrow+ncol;++i){
    vertices[i].index = i;
    if(vertices[i].p_in_edges.size()!=0)
      init_vertices.push_back(&vertices[i]);
  }

  if(init_vertices.size()==0)
    throw invalid_argument("Graph fully determined by specification");

  return;
}

edge::edge(vertex* const ph,vertex* const pt, int* const pw, const bool f):
  p_head(ph),
  p_tail(pt),
  fixed(f),
  p_weight(pw),
  pos(-1),
  m_visits(STAR)
{
  if(*p_weight>0 && !fixed) add();
  if(!fixed) p_tail->p_poss_out_edges.push_back(this);
  return;
}



void weightedGraph::sampleKernel(vector<edge*> & vec){

    // find a cycle
    vertex* u0 = sampleFromVector(init_vertices, generator);
    edge* e = sampleFromVector(u0->p_in_edges, generator);
    if(e->visits() == STAR) vec.push_back(e);
    e->increment();
    e = sampleNewFromVector(e->tail()->p_poss_out_edges, e, generator);
    if(e->visits() == STAR) vec.push_back(e);
    e->decrement();
    while(e->head()!=u0){
      e = sampleNewFromVector(e->head()->p_in_edges, e, generator);
      if(e->visits() == STAR) vec.push_back(e);
      e->increment();
      if(!edges[e->tail()->index][u0->index-edges.size()].is_fixed())
        e = &edges[e->tail()->index][u0->index-edges.size()];
      else
        e = sampleNewFromVector(e->tail()->p_poss_out_edges, e, generator);
      if(e->visits() == STAR) vec.push_back(e);
      e->decrement();
    }
    return;
}


deltaRange weightedGraph::getDeltaRange(vector<edge*> & vec){
  // compute support for Delta
  deltaRange dr;
  for(const auto &e: vec){
    if(e->visits()>0) dr.low = max(dr.low, - e->weight()/e->visits());
    if(e->visits()<0) dr.up = min(dr.up, -e->weight()/e->visits());
  }
  return dr;
}


void weightedGraph::updateWeights(vector<edge*>& vec, int delta){
  for(auto &e: vec){
    e->set_weight(e->weight()+e->visits()*delta);
    e->reset();
  }
}


void weightedGraph::sampleStep(){
  vector<edge*> cycle;
  sampleKernel(cycle);
  deltaRange dr = getDeltaRange(cycle);
  int delta = sampleDelta(dr,generator);
  updateWeights(cycle, delta);
  return;
}

void edge::increment(){
  if(m_visits==STAR) m_visits = 1;
  else m_visits++;
  return;
}

void edge::decrement(){
  if(m_visits==STAR) m_visits = -1;
  else m_visits--;
  return;
}


// adds edge to in 'in edges' of head vertex
// assumes edge is NOT already referenced in vertex data
// will only be called as part of set_weight
void edge::add(){
  pos = p_head->p_in_edges.size();
  p_head->p_in_edges.push_back(this);
}

// removes edge from 'in edges' of a vertex
// assumes edge is referenced in vertex at location pos
// will only be called as part of set_weight
void edge::remove(){
    p_head->p_in_edges.back()->set_pos(pos);
    swap(p_head->p_in_edges[pos], p_head->p_in_edges.back());
    p_head->p_in_edges.pop_back();
}

void edge::set_weight(int w){
  if(*p_weight==0 && w>0) add();
  if(*p_weight>0 && w==0) remove();
  *p_weight = w;
}


void weightedGraph::printData(){

  printMatrix(adj_matrix);
  for(int i=0; i!=vertices.size(); ++i){
    printVertexData(vertices[i]);
    Rcout << endl;
  }
  return;
}
