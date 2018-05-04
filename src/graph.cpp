

#include "graph.h"
#include "StronglyConnectedComponents.h"
#include "FeasibleMatrix.h"
#include "AuxiliaryFunctions.h"
using namespace Rcpp;
using namespace UnWeighted;
using namespace std;

/*
update_data is used to reconstruct class data from the adjacency
matrix. This is required as different sampling methods use different class
data, so it is computationally inefficient to track all data during each
sampling step.
*/

void graph::update_data(){

  // initialise the data structures
  zero_nums = vector<int>(nrow, 0);
  one_nums = vector<int>(ncol, 0);
  ones = vector<vector<int> > (ncol, vector<int>(0));
  zeros = vector<vector<int> > (nrow, vector<int>(0));
  inStubs = vector<int>(0);
  outStubs = vector<int>(0);
  EdgeList = vector<Edge>(0);
  nStubs = 0;

  // (re)construct them from the current adjacency matrix
  if(directed){
    for(int i=0;i!=nrow;++i){
      for(int j=0;j!=ncol;++j){
        if(fixed(i,j)==0){
          if(x(i,j)==0){
            zeros[i].push_back(j);
            zeroNums[i]++;
          }
          else{
            // add Edge to Edge list
            Edge e;
            e.tail = i; e.head = j;
            EdgeList.push_back(e);

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
    for(int k=0; k!=nrow; ++k){
      for(int i=0; i!=k;++i){
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
            // add Edge to Edge list
            Edge e;
            e.tail = i; e.head = k;
            EdgeList.push_back(e);
            nStubs++;
          }
        }
      }
      for(int j=(k+1);j!=nrow;++j){
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
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  generator = default_random_engine(seed);
  tracking = vector<vector<vector<int> > >(nrow, vector<vector<int> >(ncol,vector<int>(2,-1)));
  //preprocess f to ensure all neccessarily fixed values have been determined
  scc_graph G(x0,f);
  fixed = G.fixed_values(f);
  fixed = f;
  vector<double> weights(ncol);

  update_data();

  for(int j=0; j!=ncol; ++j){
    uniform_int_distribution<int> dist(0, oneNums[j]-1);
    oneSampler.push_back(dist);
    if(oneNums[j] == 0)
      weights[j] = 0.0;
    else
      weights[j] = 1.0;
  }

  for(int i=0;i!=nrow;++i){
    uniform_int_distribution<int> dist(0,zeroNums[i]-1);
    zeroSampler.push_back(dist);
  }

  one_dist = discrete_distribution<int> (weights.begin(), weights.end());

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

  if(directed){
    for(int i=0; i!=nrow;++i){
      for(int j=0; j!=zeroNums[i];++j)
        x(i,zeros[i][j]) = 0;
    }
    for(int j= 0; j!=ncol;++j){
      for(int i=0; i!=oneNums[j];++i)
        x(ones[j][i],j) = 1;
    }
  }
  else{

    for(int i=0; i!=nrow; ++i){
      for(int j=0; j!=zeroNums[i];++j){
        if(zeros[i][j]>i){
          x(i,zeros[i][j])=0;
          x(zeros[i][j],i)=0;
        }
      }
    }

    for(int i=0; i!=ncol; ++i){
      for(int j=0; j!=oneNums[i];++j){
        if(ones[i][j]>i){
          x(i,ones[i][j])=1;
          x(ones[i][j],i)=1;
        }
      }
    }
  }
}


/*
Samples and returns adjacency matrices using one of the four procedures:
"SG", "DG". "switch" and "matching".
*/


List graph::sample(string method, int nsamples=1e4, int thin = 20, int burnin = 1e4){

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
    throw range_error("Method must be either 'SG', 'DG', 'switch' or 'matching' ");

  //rejected = 0;

  srand(time(NULL));
  List results(nsamples);

  for(int i=0; i!=nsamples;++i){
    for(int j=0; j!=(thin+1);++j)
      (*this.*func)();
    if(func==&graph::SG_step)
      update_x();
    results(i) = clone(x);
  }
  // ensure internal data structures remain valid
  update_data();

  //Rcout << "Rejected: " << float (rejected)/ float ((nsamples*(thin+1))) << endl;
  return results;
}

/*
Methods below can be used to print internal class data to the R
console for sanity checks
*/

void graph::print_EdgeList(){
  for(vector<Edge>::iterator it = EdgeList.begin(); it!=EdgeList.end(); ++it)
    Rcout << "(" << it->tail +1 << ", " << it->head+1 << ")" << endl;
}

void graph::print_stub_list(){
  Rcout << "Total Stubs: " << nStubs << endl;
  Rcout << "InStubs: " << endl;
  printVec(inStubs);
  Rcout << endl << "OutStubs" << endl;
  printVec(outStubs);
  Rcout << endl;
}

void graph::print_data(){
  Rcout << "Ones: " << endl;
  for(int i=0; i!=ncol;++i){
    Rcout << "Constraint " << i+1 << ": ";
    printVec(ones[i]);
  }
  Rcout << endl;
  Rcout << "Zeros: " << endl;
  for(int i=0; i!=nrow;++i){
    Rcout << "Constraint " << i+1 << ": ";
    printVec(zeros[i]);
  }

  Rcout << endl;
  Rcout << "Tracking Matrix: " << endl;

  for(int i=0; i!=nrow;++i){
    for(int j=0; j!=ncol;++j){
      Rcout << "(" << setw(2) << tracking[i][j][0];
      Rcout << setw(1) << "," << setw(2);
      Rcout << setw(2) << tracking[i][j][1];
      Rcout << setw(1) << ") ";
    }
    Rcout << endl;
  }
  Rcout << endl;

  return;
}
