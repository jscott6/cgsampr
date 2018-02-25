

#include "btable.h"
#include "StronglyConnectedComponents.h"
#include "FeasibleMatrix.h"
#include "AuxiliaryFunctions.h"

using namespace Rcpp;

IntegerMatrix digraph::get_fixed(){return fixed;};



// initialisation for the digraph...

void digraph::init(IntegerMatrix x0, IntegerMatrix f){

  x = x0;
  nrow = x.nrow(); ncol = x.ncol();
  zeroNums = std::vector<int>(nrow+ncol,0);
  oneNums = std::vector<int>(nrow+ncol, 0);
  ones = std::vector<std::vector<int> > (nrow+ncol, std::vector<int>(0));
  zeros = std::vector<std::vector<int> > (nrow+ncol, std::vector<int>(0));
  std::vector<double> weights(ncol);
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
  for(int j=0; j<ncol; j++){

    std::uniform_int_distribution<int> dist(0, colNums[j]-1);
    colSampler.push_back(dist);

    if(colNums[j] == 0)
      weights[j] = 0.0;
    else
      weights[j] = 1.0;
  }
  col_dist = std::discrete_distribution<int> (weights.begin(), weights.end());

  for(int i=0;i<nrow;i++){
    std::uniform_int_distribution<int> dist(0,rowNums[i]-1);
    rowSampler.push_back(dist);
  }

}




/*
void digraph::init(IntegerMatrix x0, IntegerMatrix f){

  x = x0;
  nrow = x.nrow(); ncol = x.ncol();
  rowNums = std::vector<int>(nrow,0);
  colNums = std::vector<int>(ncol, 0);
  rows = std::vector<std::vector<int> > (nrow, std::vector<int>(0));
  cols = std::vector<std::vector<int> > (ncol, std::vector<int>(0));
  std::vector<double> weights(ncol);
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator = std::default_random_engine(seed);

  //preprocess f to ensure all neccessarily fixed values have been determined
  scc_graph G(x0,f);
  fixed = G.fixed_values(f);
  fixed = f;

  for(int i=0;i<nrow;i++){
    for(int j=0;j<ncol;j++){
      // only add to data structure if not known
      if(fixed(i,j) == 0){
        if(x(i,j) == 0){
          rows[i].push_back(j);
          rowNums[i]++;
        }
        else{
          cols[j].push_back(i);
          colNums[j]++;
        }
      }
    }
  }

  for(int j=0; j<ncol; j++){

    std::uniform_int_distribution<int> dist(0, colNums[j]-1);
    colSampler.push_back(dist);

    if(colNums[j] == 0)
      weights[j] = 0.0;
    else
      weights[j] = 1.0;
  }
  col_dist = std::discrete_distribution<int> (weights.begin(), weights.end());

  for(int i=0;i<nrow;i++){
    std::uniform_int_distribution<int> dist(0,rowNums[i]-1);
    rowSampler.push_back(dist);
  }
}
*/

digraph::digraph(IntegerMatrix x0, IntegerMatrix f){
  init(x0,f);
}

// constructor utilising max flow (Edmund Karps) algorithm to attempt to
// reconstruct an admissible matrix from provided marginals.
digraph::digraph(IntegerVector r, IntegerVector c, IntegerMatrix f){

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
void digraph::update_x(){
// create matrix by cycling through both rows and cols
  for(int i=0; i<nrow;i++){
    for(int j=0; j<rowNums[i];j++)
      x(i,rows[i][j]) = 0;
  }
  for(int j= 0; j<ncol;j++){
    for(int i=0; i<colNums[j];i++)
      x(cols[j][i],j) = 1;
  }
}

// generates one sample matrix
void digraph::sample_step(){

  // sample column randomly
  int j1 = col_dist(generator);
  int i1_idx = colSampler[j1](generator);
  int j = j1, i = cols[j1][i1_idx];
  int i_ind, j_ind;

  while(true){
    // sample j_k from rows i_{k-1}
    j_ind = rowSampler[i](generator);
    std::swap(j, rows[i][j_ind]);

    if(j == j1) break;

    // sample i_k from cols j_{k}
    i_ind = colSampler[j](generator);
    std::swap(i, cols[j][i_ind]);
  }
  // need to change cols i1 from j1 to jk
  cols[j1][i1_idx] = i;
}


List digraph::sample_step_show_workings(unsigned int max_size){

  // sample column randomly
  int j1 = col_dist(generator);
  int i1_idx = colSampler[j1](generator);
  int j = j1, i = cols[j1][i1_idx];
  int i1 = i;
  int i_ind, j_ind;
  List results(max_size);

  update_x();
  results(0) = clone(x);
  int l = 0;
  while(true){
    l = l+1;
    // sample j_k from rows i_{k-1}
    j_ind = rowSampler[i](generator);
    std::swap(j, rows[i][j_ind]);
    update_x();
    x(i1,j1) = 0;
    results(l) = clone(x);

    if(j == j1) break;
    l = l+1;

    // sample i_k from cols j_{k}
    i_ind = colSampler[j](generator);
    std::swap(i, cols[j][i_ind]);
    update_x();
    x(i1,j1) = 0;
    results(l) = clone(x);
  }
  // need to change cols i1 from j1 to jk
  cols[j1][i1_idx] = i;
  update_x();
  x(i1,j1) = 0;
  l = l+1;
  results(l) = clone(x);
  return results;
}

List digraph::sample(int nsamples=1e4, int thin = 20, int burnin = 1e4){

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

Rcpp::IntegerMatrix btable::get_x(){
  update_x();
  return x;
}


void digraph::print_data(){
  Rcout << "Rows with 1s: " << std::endl;
  for(int i=0; i<ncol;i++){
    Rcout << "Column " << i+1 << ": ";
    printVec(cols[i]);
  }
  Rcout << std::endl;
  Rcout << "Columns with 0s: " << std::endl;
  for(int i=0; i<nrow;i++){
    Rcout << "Row " << i+1 << ": ";
    printVec(rows[i]);
  }
  Rcout << std::endl;
  return;
}
