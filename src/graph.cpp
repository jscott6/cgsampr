

#include "graph.h"
#include "StronglyConnectedComponents.h"
#include "FeasibleMatrix.h"
#include "AuxiliaryFunctions.h"

using namespace Rcpp;

/*
update_data is used to reconstruct class data from the adjacency
matrix. This is required as different sampling methods use different class
data, so it is computationally inefficient to track all data during each
sampling step.
*/


void graph::update_data(){

  // initialise the data structures
  zeroNums = std::vector<int>(nrow,0);
  oneNums = std::vector<int>(ncol, 0);
  ones = std::vector<std::vector<int> > (nrow, std::vector<int>(0));
  zeros = std::vector<std::vector<int> > (ncol, std::vector<int>(0));
  inStubs = std::vector<int>(0);
  outStubs = std::vector<int>(0);
  arcList = std::vector<arc>(0);
  nStubs = 0;

  // (re)construct them from the current adjacency matrix
  if(directed){
    for(int i=0;i<nrow;i++){
      for(int j=0;j<ncol;j++){
        if(fixed(i,j)==0){
          if(x(i,j)==0){
            zeros[i].push_back(j);
            zeroNums[i]++;
          }
          else{
            // add arc to arc list
            arc e;
            e.tail = i; e.head = j;
            arcList.push_back(e);

            // add stubs for matching method
            inStubs.push_back(j);
            outStubs.push_back(i);
            nStubs++;

            ones[j].push_back(i);
            oneNums[j]++;
          }
        }
      }
    }
  }
  else{
    for(int k=0; k<nrow; k++){
      for(int i=0; i<k;i++){
        if(fixed(i,k)==0){
          if(x(i,k)==0){
            zeros[k].push_back(i);
            tracking[i][k][1] = zeroNums[k];
            zeroNums[k]++;
          }
          else{
            ones[k].push_back(i);
            tracking[i][k][1] = oneNums[k];
            oneNums[k]++;
          }
        }
      }
      for(int j=(k+1);j<nrow;j++){
        if(fixed(k,j)==0){
          if(x(k,j)==0){
            zeros[k].push_back(j);
            tracking[k][j][0] = zeroNums[k];
            zeroNums[k]++;
          }
          else{
            ones[k].push_back(j);
            tracking[k][j][0] = oneNums[k];
            oneNums[k]++;
          }
        }
      }
    }
  }
}

/*
Initialises class data upon calling either of the constructors
*/

void graph::init(IntegerMatrix x0, IntegerMatrix f){

  x = clone(x0);
  nrow = x.nrow(); ncol = x.ncol();
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator = std::default_random_engine(seed);
  tracking = std::vector<std::vector<std::vector<int> > >(nrow, std::vector<std::vector<int> >(ncol,std::vector<int>(2,-1)));
  //preprocess f to ensure all neccessarily fixed values have been determined
  scc_graph G(x0,f);
  fixed = G.fixed_values(f);
  fixed = f;
  std::vector<double> weights(ncol);

  update_data();

  for(int j=0; j<ncol; j++){
    std::uniform_int_distribution<int> dist(0, oneNums[j]-1);
    oneSampler.push_back(dist);
    if(oneNums[j] == 0)
      weights[j] = 0.0;
    else
      weights[j] = 1.0;
  }

  for(int i=0;i<nrow;i++){
    std::uniform_int_distribution<int> dist(0,zeroNums[i]-1);
    zeroSampler.push_back(dist);
  }

  one_dist = std::discrete_distribution<int> (weights.begin(), weights.end());

}



/*
constructs object from adjacency matrix and matrix indicating which entries
are fixed.
Fixed matrix is determined by finding strongly connected components in D_x
(procedure outlined in the paper)
*/


graph::graph(IntegerMatrix x0, IntegerMatrix f, bool digraph){
  directed = digraph;
  init(x0,f);
}

/*
constructor utilising max flow (Edmund Karps) algorithm to attempt to
reconstruct an admissible matrix from provided degree sequence. Note this
procedure will be successful if and only if the degree sequence is GRAPHICAL
*/


graph::graph(IntegerVector r, IntegerVector c, IntegerMatrix f, bool digraph){

  directed = digraph;

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


/*
Reconstructs adjacency matrix from adjacency lists zeros and ones.
Used to output adjacency matrices in the case of the SG sampler,
which does not explicitly track adjacency matrices through the sampling
procedure.
*/

// recreates x from data structure
void graph::update_x(){
// create matrix by cycling through both rows and cols
  for(int i=0; i<nrow;i++){
    for(int j=0; j<zeroNums[i];j++)
      x(i,zeros[i][j]) = 0;
  }
  for(int j= 0; j<ncol;j++){
    for(int i=0; i<oneNums[j];i++)
      x(ones[j][i],j) = 1;
  }
}


/*
Samples and returns adjacency matrices using one of the four procedures:
"SG", "DG". "switch" and "matching".
*/


List graph::sample(std::string method, int nsamples=1e4, int thin = 20, int burnin = 1e4){

  void (graph::*func)();
  if(method == "SG")
    func = &graph::SG_step;
  else if(method == "DG")
    func = &graph::DG_step;
  else if(method == "switch")
    func = &graph::switch_step;
  else if(method == "matching")
    func = &graph::matching_step;
  else
    throw std::range_error("Method must be either 'SG', 'DG', 'switch' or 'matching' ");

  srand(time(NULL));
  List results(nsamples);

  for(int i=0; i<nsamples;i++){
    for(int j=0; j<(thin+1);j++)
      (*this.*func)();
    if(func==&graph::SG_step)
      update_x();
    results(i) = clone(x);
  }
  // ensure internal data structures remain valid
  update_data();
  return results;
}


/*
getters to output current adjacency matrix and show fixed entries.
*/

IntegerMatrix graph::get_x(){ return x;}
IntegerMatrix graph::get_fixed(){return fixed;};



/*
Methods below can be used to print internal class data to the R
console for sanity checks
*/

void graph::print_arcList(){
  for(std::vector<arc>::iterator it = arcList.begin(); it!=arcList.end(); it++)
    Rcout << "(" << it->tail +1 << ", " << it->head+1 << ")" << std::endl;
}

void graph::print_stub_list(){
  Rcout << "Total Stubs: " << nStubs << std::endl;
  Rcout << "InStubs: " << std::endl;
  printVec(inStubs);
  Rcout << std::endl << "OutStubs" << std::endl;
  printVec(outStubs);
  Rcout << std::endl;
}

void graph::print_data(){
  Rcout << "Ones: " << std::endl;
  for(int i=0; i<ncol;i++){
    Rcout << "Constraint " << i+1 << ": ";
    printVec(ones[i]);
  }
  Rcout << std::endl;
  Rcout << "Zeros: " << std::endl;
  for(int i=0; i<nrow;i++){
    Rcout << "Constraint " << i+1 << ": ";
    printVec(zeros[i]);
  }

  Rcout << std::endl;
  Rcout << "Tracking Matrix: " << std::endl;

  for(int i=0; i<nrow;i++){
    for(int j=0; j<ncol;j++){
      Rcout << "(" << std::setw(2) << tracking[i][j][0];
      Rcout << std::setw(1) << "," << std::setw(2);
      Rcout << std::setw(2) << tracking[i][j][1];
      Rcout << std::setw(1) << ") ";
    }
    Rcout << std::endl;
  }
  Rcout << std::endl;

  return;
}
