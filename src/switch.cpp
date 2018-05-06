
/*
------------------------------------------------------------
Simple implementation of "Switch" algorithm for directed_
graphs.
------------------------------------------------------------
*/

#include "graph.h"

using namespace Rcpp;
using namespace UnWeighted;


void Graph::switchStep(){

  if(edge_list_.size()-1== 0)
    throw std::range_error("Only one arc/edge, not possible to sample");

  if(directed_){

    int idx1, idx2;
    std::uniform_int_distribution<int> dist(0, edge_list_.size()-1);

    idx1 = dist(generator_);
    idx2 = dist(generator_);

    // edges must be distinct
    while(idx1 == idx2)
      idx2 = dist(generator_);

    int head1 = edge_list_[idx1].head;
    int head2 = edge_list_[idx2].head;
    int tail1 = edge_list_[idx1].tail;
    int tail2 = edge_list_[idx2].tail;


    // no edges can already exist between heads and tails of different edges
    if(adjacency_matrix_(tail2, head1) == 1 || adjacency_matrix_(tail1,head2) == 1){
      rejected_++;
      return;
    }

    // potential edges cannot be fixed_...
    if(fixed_(tail2, head1)==1 || fixed_(tail1,head2)==1){
      rejected_++;
      return;
    }

    // update edges
    edge_list_[idx1].head = head2;
    edge_list_[idx2].head = head1;

    // update adjacency_matrix_
    adjacency_matrix_(tail1,head1) = 0;
    adjacency_matrix_(tail2,head1) = 1;
    adjacency_matrix_(tail1,head2) = 1;
    adjacency_matrix_(tail2,head2) = 0;

  }
  else{

    int idx1, idx2;
    std::uniform_int_distribution<int> dist(0, edge_list_.size()-1);

    idx1 = dist(generator_);
    idx2 = dist(generator_);

    // edges must be distinct
    while(idx1 == idx2)
      idx2 = dist(generator_);

    int y = edge_list_[idx1].head;
    int z = edge_list_[idx1].tail;
    int u = edge_list_[idx2].head;
    int v = edge_list_[idx2].tail;

    // if proposed edges are prohibited, return
    if(fixed_(y,v)==1 ||fixed_(z,u)==1){
      rejected_++;
      return;
    }

    // ... or if proposed edges exist, return
    if(adjacency_matrix_(y,v)==1 || adjacency_matrix_(z,u)==1){
      rejected_++;
      return;
    }

    edge_list_[idx1].tail = v;
    edge_list_[idx2].head = u;

    // remove existing edges
    adjacency_matrix_(y,z) = 0;
    adjacency_matrix_(z,y) = 0;
    adjacency_matrix_(u,v) = 0;
    adjacency_matrix_(v,u) = 0;

    // add new edges
    adjacency_matrix_(y,v) = 1;
    adjacency_matrix_(v,y) = 1;
    adjacency_matrix_(z,u) = 1;
    adjacency_matrix_(u,z) = 1;


  }


  return;
}
