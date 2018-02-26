
void graph::matching_step(){

  int discard = 0;
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
    int head = inStubs[inIdx];
    int tail = outStubs[outIdx];

    // discard if self-loop or arc exists already
    if(head==tail || x(tail,head)==1){
      discard++;
    }
    else{
      x(tail,head)=1;
      // swap with final entry
      std::swap(inStubs[inIdx], inStubs[nFreeStubs]);
      nFreeStubs--;
    }
  }

}
