

#include "graph_switch.h"
#include "StronglyConnectedComponents.h"
#include "FeasibleMatrix.h"
#include "AuxiliaryFunctions.h"

using namespace Rcpp;

IntegerMatrix graph_switch::get_fixed(){return fixed;};
Rcpp::IntegerMatrix graph_switch::get_x(){return x;}


void graph_switch::init(IntegerMatrix x0, IntegerMatrix f){

  x = clone(x0);
  int nrow = x.nrow(); int ncol = x.ncol();
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator = std::default_random_engine(seed);
  Nstubs = 0;

  scc_graph G(x0,f);
  fixed = G.fixed_values(f);
  fixed = f;


  for(int i=0; i<nrow; i++){
    for(int j=0; j<ncol; j++){
      if(x(i,j)==1){
        instubs.push_back(j);
        outstubs.push_back(i);
        Nstubs++;
      }
    }
  }

}

void graph_switch::init(IntegerMatrix x0, IntegerMatrix f){

  x = clone(x0);
  int nrow = x.nrow(); int ncol = x.ncol();
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator = std::default_random_engine(seed);

  scc_graph G(x0,f);
  fixed = G.fixed_values(f);
  fixed = f;

  for(int i=0; i<nrow; i++){
    for(int j=0; j<ncol; j++){
      if(x(i,j)==1){
        arc e;
        e.head = i; e.tail = j;
        arc_list.push_back(e);
      }
    }
  }

}

graph_switch::graph_switch(IntegerMatrix x0, IntegerMatrix f){
  init(x0, f);
}


// employs switch method to sample a single step
void graph_switch::switch_sample_step(){

  int idx1, idx2;
  std::uniform_int_distribution<int> dist(0, arc_list.size());

  idx1 = dist(generator);
  idx2 = dist(generator);

  // edges must be distinct
  while(idx1 == idx2)
    idx2 = dist(generator);

  int head1 = arc_list[idx1].head;
  int head2 = arc_list[idx2].head;
  int tail1 = arc_list[idx1].tail;
  int tail2 = arc_list[idx2].tail;

  // delineated vertices must be distinct
  if(head1==head2 || head1==tail2 || head2==tail1 || tail1==tail2){
    return;
  }
  // no edges can already exist between heads and tails of different edges
  if(x(head1,tail2) == 1 || x(head2,tail1) == 1)
    return;

  // potential edges cannot be fixed...
  if(fixed(head1,tail2)==1 || fixed(head2,tail1)==1)
    return;


  Rcout << "(" << head1+1 << ", " << tail1+1 << ")" << std::endl;
  Rcout << "(" << head2+1 << ", " << tail2+1 << ")" << std::endl;

  // update edges
  arc_list[idx1].tail = tail2;
  arc_list[idx2].tail = tail1;

  // update x
  x(head1,tail1) = 0;
  x(head1,tail2) = 1;
  x(head2,tail1) = 1;
  x(head2,tail2) = 0;

  return;
}

// completely state-independent sampling procedure
void graph_switch::submatrix_sample_step(){

  std::uniform_int_distribution<int> row_dist(0, nrow);
  std::uniform_int_distribution<int> col_dist(0, ncol);

  i1 = row_dist(generator);
  i2 = row_dist(generator);
  j1 = col_dist(generator);
  j2 = col_dist(generator);

  // ensure we've delineated two row and columns
  if(i1==i2 || j1 == j2)
    return;
  // return if any delineated element is fixed
  if(fixed(i1,j1)==1 || fixed(i1,j2)==1||fixed(i2,j1)==1||fixed(i2,j2)==1)
    return;

  // only alter the adjacency matrix if one of the following two patterns is observed
  // pattern 1
  if(x(i1,j1)==1 && x(i1,j2)==0 && x(i2,j1)==0 && x(i2,j2)==1){
    x(i1,j1)==0;
    x(i1,j2)==1;
    x(i2,j1)==1;
    x(i2,j2)==0;
  }
  // pattern 2
  if(x(i1,j1)==0 && x(i1,j2)==1 && x(i2,j1)==1 && x(i2,j2)==0){
    x(i1,j1)==1;
    x(i1,j2)==0;
    x(i2,j1)==0;
    x(i2,j2)==1;
  }
  return;
}



void graph_switch::print_arc_list(){
  for(std::vector<arc>::iterator it = arc_list.begin(); it!=arc_list.end(); it++)
    Rcout << "(" << it->head +1 << ", " << it->tail+1 << ")" << std::endl;
}



void graph_switch::stubs_sample_step(){

  int discard == 0;
  int nFreeStubs = nStubs;

  // initialise adjacency matrix to zero matrix
  for(int i=0; i<nrow; i++){
    for(int j=0; j<ncol; j++){
      x(i,j)=0;
    }
  }

  while(nFreeStubs>0){

    std::uniform_int_distribution<int> dist(0,nFreeStubs);
    int inIdx = dist(generator);
    int outIdx = dist(generator);
    int head = instubs[inIdx];
    int tail = outstubs[outIdx];

    // discard if self-loop or arc exists already
    if(head==tail || x(tail,head)==1){
      discard++
    }
    else{
      x(tail,head)=1;
      // swap with final entry
      std::swap(instubs[inIdx], instubs[nFreeStubs]);
      nFreeStubs--;
    }
  }


}
