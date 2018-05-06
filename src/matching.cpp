
#include "graph.h"

using namespace Rcpp;
using namespace UnWeighted;

void Graph::matchingStep(){


  int attempts = 0;
  int max_attempts = 1e5;
  int r = 10;

  while(true){

    attempts++;
    int discard = 0;
    int nFreeStubs = nstubs_;

    // initialise adjacency matrix to zero matrix
    for(int i=0; i!=nrow_; ++i){
      for(int j=0; j!=ncol_; ++j){
        if(fixed_(i,j)==0){
          adjacency_matrix_(i,j)=0;
        }
      }
    }

    while(true){

      std::uniform_int_distribution<int> dist(0,nFreeStubs-1);
      int inIdx = dist(generator_);
      int outIdx = dist(generator_);
      int head = in_stubs_[inIdx];
      int tail = out_stubs_[outIdx];

      // discard if self-loop or arc exists already
      if(head==tail || adjacency_matrix_(tail,head)==1 || fixed_(tail,head)==1){
        discard++;
      }
      else{
        adjacency_matrix_(tail,head)=1;
        // swap with final entry
        if(inIdx!=(nFreeStubs-1))
          std::swap(in_stubs_[inIdx], in_stubs_[nFreeStubs-1]);
        if(outIdx!=(nFreeStubs-1))
          std::swap(out_stubs_[outIdx], out_stubs_[nFreeStubs-1]);
        nFreeStubs--;
      }

      if(nFreeStubs==0)
        return;

      if(discard > r*nstubs_)
        break;
    }

    if(attempts == max_attempts)
      throw std::string("Could not construct a viable Graph");

  }

}
