
/*
------------------------------------------------------------
Simple implementation of "Switch" algorithm for directed
graphs.
------------------------------------------------------------
*/

#include "graph.h"

using namespace Rcpp;


void graph::switch_step(){

  if(arcList.size()-1)== 0)
    return;

  if(directed){

    int idx1, idx2;
    std::uniform_int_distribution<int> dist(0, arcList.size()-1);

    idx1 = dist(generator);
    idx2 = dist(generator);

    // edges must be distinct
    while(idx1 == idx2)
      idx2 = dist(generator);

    int head1 = arcList[idx1].head;
    int head2 = arcList[idx2].head;
    int tail1 = arcList[idx1].tail;
    int tail2 = arcList[idx2].tail;


    // no edges can already exist between heads and tails of different edges
    if(x(tail2, head1) == 1 || x(tail1,head2) == 1)
      return;

    // potential edges cannot be fixed...
    if(fixed(tail2, head1)==1 || fixed(tail1,head2)==1)
      return;

    // update edges
    arcList[idx1].head = head2;
    arcList[idx2].head = head1;

    // update x
    x(tail1,head1) = 0;
    x(tail2,head1) = 1;
    x(tail1,head2) = 1;
    x(tail2,head2) = 0;

  }
  else{

    int idx1, idx2;
    std::uniform_int_distribution<int> dist(0, arcList.size()-1);

    idx1 = dist(generator);
    idx2 = dist(generator);

    // edges must be distinct
    while(idx1 == idx2)
      idx2 = dist(generator);

    int y = arcList[idx1].head;
    int z = arcList[idx1].tail;
    int u = arcList[idx2].head;
    int v = arcList[idx2].tail;

    // if proposed edges are prohibited, return
    if(fixed(y,v)==1 ||fixed(z,u)==1)
      return;

    // ... or if proposed edges exist, return
    if(x(y,v)==1 || x(z,u)==1)
      return;

    arcList[idx1].tail = v;
    arcList[idx2].head = u;

    // remove existing edges
    x(y,z) = 0;
    x(z,y) = 0;
    x(u,v) = 0;
    x(v,u) = 0;

    // add new edges
    x(y,v) = 1;
    x(v,y) = 1;
    x(z,u) = 1;
    x(u,z) = 1;


  }


  return;
}
