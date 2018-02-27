
/*
------------------------------------------------------------
Simple implementation of "Switch" algorithm for directed
graphs.
------------------------------------------------------------
*/

#include "graph.h"

using namespace Rcpp;


void graph::switch_step(){

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

  // delineated vertices must be distinct
  if(head1==head2 || head1==tail2 || head2==tail1 || tail1==tail2){
    return;
  }
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

  return;
}
