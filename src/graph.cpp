

#include "graph.h"
#include "StronglyConnectedComponents.h"
#include "FeasibleMatrix.h"
#include "AuxiliaryFunctions.h"

using namespace Rcpp;

IntegerMatrix graph::get_fixed(){return fixed;};

// initialisation for the digraph...

void graph::init(IntegerMatrix x0, IntegerMatrix f){


  x = clone(x0);
  nrow = x.nrow(); ncol = x.ncol();
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator = std::default_random_engine(seed);
  //preprocess f to ensure all neccessarily fixed values have been determined
  scc_graph G(x0,f);
  fixed = G.fixed_values(f);
  fixed = f;

  zeroNums = std::vector<int>(nrow+ncol,0);
  oneNums = std::vector<int>(nrow+ncol, 0);
  ones = std::vector<std::vector<int> > (nrow+ncol, std::vector<int>(0));
  zeros = std::vector<std::vector<int> > (nrow+ncol, std::vector<int>(0));
  std::vector<double> weights(nrow+ncol);
  nStubs = 0;

  for(int i=0; i<nrow; i++){
    for(int j=0; j<ncol; j++){
      if(x(i,j)==1){
        inStubs.push_back(j);
        outStubs.push_back(i);
        nStubs++;
      }
    }
  }

  for(int i=0; i<nrow; i++){
    for(int j=0; j<ncol; j++){
      if(x(i,j)==1){
        arc e;
        e.head = i; e.tail = j;
        arc_list.push_back(e);
      }
    }
  }

  for(int i=0;i<nrow;i++){
    for(int j=0;j<ncol;j++){
      if(fixed(i,j)==0){
        if(x(i,j)==0){
          zeros[i].push_back(nrow+j);
          zeroNums[i]++;
          zeros[nrow+j].push_back(i);
          zeroNums[nrow+j]++;
        }
        else{
          ones[i].push_back(nrow+j);
          oneNums[i]++;
          ones[nrow+j].push_back(i);
          oneNums[nrow+j]++;
        }
      }
    }
  }

  for(int i=0; i<nrow+ncol; i++){

    std::uniform_int_distribution<int> dist(0, oneNums[i]-1);
    oneSampler.push_back(dist);
    if(oneNums[i] == 0)
      weights[i] = 0.0;
    else
      weights[i] = 1.0;
  }

  for(int i=0; i<nrow+ncol; i++){
    std::uniform_int_distribution<int> dist(0, zeroNums[i]-1);
    zeroSampler.push_back(dist);
  }

  one_dist = std::discrete_distribution<int> (weights.begin(), weights.end());

}


graph::graph(IntegerMatrix x0, IntegerMatrix f){
  init(x0,f);
}

// constructor utilising max flow (Edmund Karps) algorithm to attempt to
// reconstruct an admissible matrix from provided marginals.
graph::graph(IntegerVector r, IntegerVector c, IntegerMatrix f){

  IntegerMatrix x0(r.size(),c.size());
  bool sinkfound=true;
  fm_graph G(r,c);
  // adjust flow until no path is found in residual graph
  while(sinkfound){
    sinkfound = G.find_path();
    G.update_flow(G.calc_path_flow());
  }
  // form matrix from graph
  x0 = G.construct_matrix(r,c);
  init(x0, f);
}


// employs switch method to sample a single step
void graph::switch_step(){

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


// employs switch method to sample a single step
void graph::DG_step(){

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

void graph::print_arc_list(){
  for(std::vector<arc>::iterator it = arc_list.begin(); it!=arc_list.end(); it++)
    Rcout << "(" << it->head +1 << ", " << it->tail+1 << ")" << std::endl;
}


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



// recreates x from data structure
void graph::update_x(){
// create matrix by cycling through both zeros and ones
  for(int i=0; i<nrow;i++){
    for(int j=0; j<zeroNums[i];j++)
      x(i,zeros[i][j]-nrow) = 0;
  }
  for(int i=0; i<nrow;i++){
    for(int j=0; j<oneNums[i];j++)
      x(i,ones[i][j]-nrow) = 1;
  }
}


void graph::SG_step(){

  // sample column randomly
  int j1 = one_dist(generator);
  int i1_idx = oneSampler[j1](generator);
  int j = j1, i = ones[j1][i1_idx];
  int i_ind, j_ind;

  while(true){
    // sample j_k from zeros i_{k-1}
    j_ind = zeroSampler[i](generator);
    std::swap(j, zeros[i][j_ind]);

    if(j == j1) break;

    // sample i_k from ones j_{k}
    i_ind = oneSampler[j](generator);
    std::swap(i, ones[j][i_ind]);
  }
  // need to change ones i1 from j1 to jk
  ones[j1][i1_idx] = i;
}

// employ one of the methods based on initialisation of graph
void graph::sample_step(){


}


List graph::sample_step_show_workings(unsigned int max_size){

  // sample column randomly
  int j1 = one_dist(generator);
  int i1_idx = oneSampler[j1](generator);
  int j = j1, i = ones[j1][i1_idx];
  int i1 = i;
  int i_ind, j_ind;
  List results(max_size);

  update_x();
  results(0) = clone(x);
  int l = 0;
  while(true){
    l = l+1;
    // sample j_k from zeros i_{k-1}
    j_ind = zeroSampler[i](generator);
    std::swap(j, zeros[i][j_ind]);
    update_x();
    x(i1,j1) = 0;
    results(l) = clone(x);

    if(j == j1) break;
    l = l+1;

    // sample i_k from ones j_{k}
    i_ind = oneSampler[j](generator);
    std::swap(i, ones[j][i_ind]);
    update_x();
    x(i1,j1) = 0;
    results(l) = clone(x);
  }
  // need to change ones i1 from j1 to jk
  ones[j1][i1_idx] = i;
  update_x();
  x(i1,j1) = 0;
  l = l+1;
  results(l) = clone(x);
  return results;
}

List graph::sample(int nsamples=1e4, int thin = 20, int burnin = 1e4){

  srand(time(NULL));
  List results(nsamples);

  for(int i=0; i<nsamples;i++){
    for(int j=0; j<thin;j++)
      sample_step();
    update_x();
    results(i) = clone(x);
  }
  return results;
}

Rcpp::IntegerMatrix graph::get_x(){
  update_x();
  return x;
}


void graph::print_data(){
  Rcout << "Ones: " << std::endl;
  for(int i=0; i<nrow+ncol;i++){
    Rcout << "Constraint " << i+1 << ": ";
    printVec(ones[i]);
  }
  Rcout << std::endl;
  Rcout << "Zeros: " << std::endl;
  for(int i=0; i<nrow+ncol;i++){
    Rcout << "Constraint " << i+1 << ": ";
    printVec(zeros[i]);
  }
  Rcout << std::endl;
  return;
}
