
#include "graph.h"

using namespace Rcpp;

void graph::matching_step(){


  int attempts = 0;
  int max_attempts = 1e5;
  int r = 10;

  while(true){

    attempts++;
    int discard = 0;
    int nFreeStubs = nStubs;

    // initialise adjacency matrix to zero matrix
    for(int i=0; i<nrow; i++){
      for(int j=0; j<ncol; j++){
        if(fixed(i,j)==0){
          x(i,j)=0;
        }
      }
    }

    while(true){

      std::uniform_int_distribution<int> dist(0,nFreeStubs-1);
      int inIdx = dist(generator);
      int outIdx = dist(generator);
      int head = inStubs[inIdx];
      int tail = outStubs[outIdx];

      // discard if self-loop or arc exists already
      if(head==tail || x(tail,head)==1 || fixed(tail,head)==1){
        discard++;
      }
      else{
        x(tail,head)=1;
        // swap with final entry
        if(inIdx!=(nFreeStubs-1))
          std::swap(inStubs[inIdx], inStubs[nFreeStubs-1]);
        if(outIdx!=(nFreeStubs-1))
          std::swap(outStubs[outIdx], outStubs[nFreeStubs-1]);
        nFreeStubs--;
      }

      if(nFreeStubs==0)
        return;

      if(discard > r*nStubs)
        break;
    }

    if(attempts == max_attempts)
      throw std::string("Could not construct a viable graph");

  }

}
