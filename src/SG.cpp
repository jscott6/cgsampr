
/*
------------------------------------------------------------
Implementation of SG algorithm.
------------------------------------------------------------
*/

#include "graph.h"

using namespace Rcpp;
using namespace UnWeighted;



void Graph::Graph(IM adjacency_matrix, IM fixed)
  : Graph(adjacency_matrix, fixed),
    vertices_(std::vector<Vertex>(adjacency_matrix.ncol() + adjacency_matrix.nrow()))
{
  nrow = adjacency_matrix.nrow(); ncol = adjacency_matrix.ncol()
  edges_ = (Edge**) malloc(nrow*sizeof(Edge));
  for (int i = 0; i != nrow; ++i) edges_[i] = (Edge*) malloc(ncol*sizeof(Edge));
  // initialise edges
  // applies constructor directly to final address (avoids copy constructor)
  // important to keep correct pointers in Vertex structures
  for (int i = 0; i != nrow; ++i)
    for (int j = 0; j != ncol; ++j)
      new (&edges_[i][j]) Edge(&vertices_[nrow+j],&vertices_[i], fixed(i,j));

  for (int i = 0; i != nrow + ncol; ++i) {
    vertices_[i].index = i;
    if (vertices_[i].in_edges.size() != 0)
      initial_vertices_.push_back(&vertices_[i]);
  }
  if (initial_vertices_.size() == 0)
    throw invalid_argument("Graph fully determined by specification");
  return;
}


// complete sampling step HERE!
void sampleStep() {
  Vertex* u0 = sampleFromVector(initial_vertices_, generator_);
  Edge* e = sampleFromVector(u0->in_edges, generator_);

}



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
void updateAdjacencyMatrix() {

};










void Graph::SGStep(){

  // initialise
  int io0 = one_dist_(generator_);
  int io0_idx = one_sampler_[io0](generator_);
  int idx, io = io0, iz = ones_[io0][io0_idx];

  if(directed_){

    while(true){

      idx = zero_sampler_[iz](generator_);
      std::swap(io, zeros_[iz][idx]);

      if(io == io0) break;

      idx = one_sampler_[io](generator_);
      std::swap(iz, ones_[io][idx]);
    }
    ones_[io0][io0_idx] = iz;
  }
  else{

    int* t0, *t;

    if(io<iz){
      t0 = &tracking_[io][iz][0];
      t = &tracking_[io][iz][1];
    }
    else{
      t0 = &tracking_[iz][io][1];
      t = &tracking_[iz][io][0];
    }

    while(true){

      idx = zero_sampler_[iz](generator_);
      ones_[iz][*t] = zeros_[iz][idx];
      std::swap(io,zeros_[iz][idx]);
      if(iz<io){
        std::swap(*t, tracking_[iz][io][0]);
        t = &tracking_[iz][io][1];
      }
      else{
        std::swap(*t, tracking_[io][iz][1]);
        t = &tracking_[io][iz][0];
      }

      if(io == io0) break;

      idx = one_sampler_[io](generator_);
      zeros_[io][*t] = ones_[io][idx];
      std::swap(iz,ones_[io][idx]);
      if(io<iz){
        std::swap(*t, tracking_[io][iz][0]);
        t = &tracking_[io][iz][1];
      }
      else{
        std::swap(*t, tracking_[iz][io][1]);
        t = &tracking_[iz][io][0];
      }
    }
    if(iz != ones_[io0][io0_idx]){
      std::swap(ones_[io0][io0_idx], zeros_[io0][*t]);
      std::swap(*t0,*t);
    }
  }
}



/*

void Graph::SGStep(){

  // initialise
  int i0 = one_dist_(generator_);
  int i0_idx = one_sampler_[i0](generator_);
  int idx, ip = i0, i = ones_[i0][i0_idx];

  if(directed_){
    // sample column randomly
    int j1 = one_dist_(generator_);
    int i1_idx = one_sampler_[j1](generator_);
    int j = j1, i = ones_[j1][i1_idx];
    int i_ind, j_ind;

    while(true){
      // sample j_k from zeros_ i_{k-1}
      j_ind = zero_sampler_[i](generator_);
      std::swap(j, zeros_[i][j_ind]);

      if(j == j1) break;

      // sample i_k from ones_ j_{k}
      i_ind = one_sampler_[j](generator_);
      std::swap(i, ones_[j][i_ind]);
    }
    // need to change ones_ i1 from j1 to jk
    ones_[j1][i1_idx] = i;
  }
  else{

    int* t0, *t;

    if(ip<i){
      t0 = &tracking_[ip][i][0];
      t = &tracking_[ip][i][1];
    }
    else{
      t0 = &tracking_[i][ip][1];
      t = &tracking_[i][ip][0];
    }

    while(true){

      idx = zero_sampler_[i](generator_);
      ip = i;
      i = zeros_[i][idx];
      // swap values
      std::swap(ones_[ip][*t],zeros_[ip][idx]);
      if(ip<i){
        std::swap(*t, tracking_[ip][i][0]);
        t = &tracking_[ip][i][1];
      }
      else{
        std::swap(*t, tracking_[i][ip][1]);
        t = &tracking_[i][ip][0];
      }

      if(i == i0) break;

      idx = one_sampler_[i](generator_);
      ip = i;
      i = ones_[i][idx];

      std::swap(zeros_[ip][*t],ones_[ip][idx]);
      if(ip<i){
        std::swap(*t, tracking_[ip][i][0]);
        t = &tracking_[ip][i][1];
      }
      else{
        std::swap(*t, tracking_[i][ip][1]);
        t = &tracking_[i][ip][0];
      }
    }

    if(ip != ones_[i0][i0_idx]){
      std::swap(ones_[i0][i0_idx], zeros_[i0][*t]);
      std::swap(*t0,*t);
    }
  }
}
*/

/*
List Graph::sample_step_show_workings(unsigned int max_size){

  // sample column randomly
  int j1 = one_dist_(generator_);
  int i1_idx = one_sampler_[j1](generator_);
  int j = j1, i = ones_[j1][i1_idx];
  int i1 = i;
  int i_ind, j_ind;
  List results(max_size);

  updateAdjacencyMatrix();
  results(0) = clone(x);
  int l = 0;
  while(true){
    l = l+1;
    // sample j_k from zeros_ i_{k-1}
    j_ind = zero_sampler_[i](generator_);
    std::swap(j, zeros_[i][j_ind]);
    updateAdjacencyMatrix();
    x(i1,j1) = 0;
    results(l) = clone(x);

    if(j == j1) break;
    l = l+1;

    // sample i_k from ones_ j_{k}
    i_ind = one_sampler_[j](generator_);
    std::swap(i, ones_[j][i_ind]);
    updateAdjacencyMatrix();
    x(i1,j1) = 0;
    results(l) = clone(x);
  }
  // need to change ones_ i1 from j1 to jk
  ones_[j1][i1_idx] = i;
  updateAdjacencyMatrix();
  x(i1,j1) = 0;
  l = l+1;
  results(l) = clone(x);
  return results;
}
*/
