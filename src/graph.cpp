

#include "graph.h"
#include "StronglyConnectedComponents.h"
#include "FeasibleMatrix.h"
#include "AuxiliaryFunctions.h"

using namespace Rcpp;

IntegerMatrix graph::get_fixed(){return fixed;};



// initialisation for the digraph...

void graph::init(IntegerMatrix x0, IntegerMatrix f){

  x = x0;
  nrow = x.nrow(); ncol = x.ncol();
  zeroNums = std::vector<int>(nrow+ncol,0);
  oneNums = std::vector<int>(nrow+ncol, 0);
  ones = std::vector<std::vector<int> > (nrow+ncol, std::vector<int>(0));
  zeros = std::vector<std::vector<int> > (nrow+ncol, std::vector<int>(0));
  std::vector<double> weights(nrow+ncol);
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator = std::default_random_engine(seed);

  //preprocess f to ensure all neccessarily fixed values have been determined
  scc_graph G(x0,f);
  fixed = G.fixed_values(f);
  fixed = f;

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
    std::uniform_int_distribution<int> dist(0, zeroNums[i]-1);
    zeroSampler.push_back(dist);
    if(oneNums[i] == 0)
      weights[i] = 0.0;
    else
      weights[i] = 1.0;

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


// recreates x from data structure
void graph::update_x(){
// create matrix by cycling through both zeros and ones
  for(int i=0; i<nrow;i++){
    for(int j=0; j<zeroNums[i];j++)
      x(i,zeros[i][j]-nrow+1) = 0;
  }
  for(int i=0; i<nrow;i++){
    for(int j=0; j<oneNums[i];j++)
      x(i,ones[i][j]-nrow+1) = 1;
  }
}

// generates one sample matrix
void graph::sample_step(){

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
