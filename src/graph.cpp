

#include "graph.h"
#include "StronglyConnectedComponents.h"
#include "FeasibleMatrix.h"
#include "AuxiliaryFunctions.h"

using namespace Rcpp;
using namespace UnWeighted;

/*
updateData is used to reconstruct class data from the adjacency
matrix. This is required as different sampling methods use different class
data, so it is computationally inefficient to track all data during each
sampling step.
*/

void Graph::updateData(){

  // initialise the data structures
  zero_nums_ = std::vector<int>(nrow_,0);
  one_nums_ = std::vector<int>(ncol_, 0);
  ones_ = std::vector<std::vector<int> > (ncol_, std::vector<int>(0));
  zeros_ = std::vector<std::vector<int> > (nrow_, std::vector<int>(0));
  in_stubs_ = std::vector<int>(0);
  out_stubs_ = std::vector<int>(0);
  edge_list_ = std::vector<Edge>(0);
  nstubs_ = 0;

  // (re)construct them from the current adjacency matrix
  if(directed_){
    for(int i=0;i!=nrow_;++i){
      for(int j=0;j!=ncol_;++j){
        if(fixed_(i,j)==0){
          if(adjacency_matrix_(i,j)==0){
            zeros_[i].push_back(j);
            zero_nums_[i]++;
          }
          else{
            // add Edge to Edge list
            Edge e;
            e.tail = i; e.head = j;
            edge_list_.push_back(e);

            // add stubs for matching method
            in_stubs_.push_back(j);
            out_stubs_.push_back(i);
            nstubs_++;

            ones_[j].push_back(i);
            one_nums_[j]++;
          }
        }
      }
    }
  }
  else{
    for(int k=0; k!=nrow_; ++k){
      for(int i=0; i!=k;++i){
        if(fixed_(i,k)==0){
          if(adjacency_matrix_(i,k)==0){
            zeros_[k].push_back(i);
            tracking_[i][k][1] = zero_nums_[k];
            zero_nums_[k]++;
          }
          else{
            ones_[k].push_back(i);
            tracking_[i][k][1] = one_nums_[k];
            one_nums_[k]++;
            // add Edge to Edge list
            Edge e;
            e.tail = i; e.head = k;
            edge_list_.push_back(e);
            nstubs_++;
          }
        }
      }
      for(int j=(k+1);j!=nrow_;++j){
        if(fixed_(k,j)==0){
          if(adjacency_matrix_(k,j)==0){
            zeros_[k].push_back(j);
            tracking_[k][j][0] = zero_nums_[k];
            zero_nums_[k]++;
          }
          else{
            ones_[k].push_back(j);
            tracking_[k][j][0] = one_nums_[k];
            one_nums_[k]++;
          }
        }
      }
    }
  }
}

/*
Initialises class data upon calling either of the constructors
*/

void Graph::init(IntegerMatrix x0, IntegerMatrix f){

  adjacency_matrix_ = clone(x0);
  nrow_ = adjacency_matrix_.nrow(); ncol_ = adjacency_matrix_.ncol();
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator_ = std::default_random_engine(seed);
  tracking_ = std::vector<std::vector<std::vector<int> > >(nrow_, std::vector<std::vector<int> >(ncol_,std::vector<int>(2,-1)));
  //preprocess f to ensure all neccessarily fixed_ values have been determined
  StronglyConnectedComponents::Graph G(x0,f);
  f = G.fixed_values(f);
  fixed_ = f;
  std::vector<double> weights(ncol_);

  updateData();

  for(int j=0; j!=ncol_; ++j){
    std::uniform_int_distribution<int> dist(0, one_nums_[j]-1);
    one_sampler_.push_back(dist);
    if(one_nums_[j] == 0)
      weights[j] = 0.0;
    else
      weights[j] = 1.0;
  }

  for(int i=0;i!=nrow_;++i){
    std::uniform_int_distribution<int> dist(0,zero_nums_[i]-1);
    zero_sampler_.push_back(dist);
  }

  one_dist_ = std::discrete_distribution<int> (weights.begin(), weights.end());

}





// recreates adjacency_matrix_ from data structure
void Graph::updateAdjacencyMatrix(){

  if(directed_){
    for(int i=0; i!=nrow_;++i){
      for(int j=0; j!=zero_nums_[i];++j)
        adjacency_matrix_(i,zeros_[i][j]) = 0;
    }
    for(int j= 0; j!=ncol_;++j){
      for(int i=0; i!=one_nums_[j];++i)
        adjacency_matrix_(ones_[j][i],j) = 1;
    }
  }
  else{

    for(int i=0; i!=nrow_; ++i){
      for(int j=0; j!=zero_nums_[i];++j){
        if(zeros_[i][j]>i){
          adjacency_matrix_(i,zeros_[i][j])=0;
          adjacency_matrix_(zeros_[i][j],i)=0;
        }
      }
    }

    for(int i=0; i!=ncol_; ++i){
      for(int j=0; j!=one_nums_[i];++j){
        if(ones_[i][j]>i){
          adjacency_matrix_(i,ones_[i][j])=1;
          adjacency_matrix_(ones_[i][j],i)=1;
        }
      }
    }
  }
}


/*
Samples and returns adjacency matrices using one of the four procedures:
"SG", "DG". "switch" and "matching".
*/


List Graph::sample(std::string method, int nsamples=1e4, int thin = 20, int burnin = 1e4){

  void (Graph::*func)();
  if(method == "SG")
    func = &Graph::SGStep;
  else if(method == "DG")
    func = &Graph::DGStep;
  else if(method == "switch")
    func = &Graph::switchStep;
  else if(method == "matching")
    func = &Graph::matchingStep;
  else
    throw std::range_error("Method must be either 'SG', 'DG', 'switch' or 'matching' ");

  //rejected_ = 0;

  srand(time(NULL));
  List results(nsamples);

  for(int i=0; i!=nsamples;++i){
    for(int j=0; j!=(thin+1);++j)
      (*this.*func)();
    if(func==&Graph::SGStep)
      updateAdjacencyMatrix();
    results(i) = clone(adjacency_matrix_);
  }
  // ensure internal data structures remain valid
  updateData();

  //Rcout << "Rejected: " << float (rejected_)/ float ((nsamples*(thin+1))) << std::endl;
  return results;
}


/*
Methods below can be used to print internal class data to the R
console for sanity checks
*/

void Graph::printEdgeList(){
  for(std::vector<Edge>::iterator it = edge_list_.begin(); it!=edge_list_.end(); ++it)
    Rcout << "(" << it->tail +1 << ", " << it->head+1 << ")" << std::endl;
}

void Graph::printStubList(){
  Rcout << "Total Stubs: " << nstubs_ << std::endl;
  Rcout << "InStubs: " << std::endl;
  printVec(in_stubs_);
  Rcout << std::endl << "OutStubs" << std::endl;
  printVec(out_stubs_);
  Rcout << std::endl;
}

void Graph::printData(){
  Rcout << "Ones: " << std::endl;
  for(int i=0; i!=ncol_;++i){
    Rcout << "Constraint " << i+1 << ": ";
    printVec(ones_[i]);
  }
  Rcout << std::endl;
  Rcout << "Zeros: " << std::endl;
  for(int i=0; i!=nrow_;++i){
    Rcout << "Constraint " << i+1 << ": ";
    printVec(zeros_[i]);
  }

  Rcout << std::endl;
  Rcout << "Tracking Matrix: " << std::endl;

  for(int i=0; i!=nrow_;++i){
    for(int j=0; j!=ncol_;++j){
      Rcout << "(" << std::setw(2) << tracking_[i][j][0];
      Rcout << std::setw(1) << "," << std::setw(2);
      Rcout << std::setw(2) << tracking_[i][j][1];
      Rcout << std::setw(1) << ") ";
    }
    Rcout << std::endl;
  }
  Rcout << std::endl;

  return;
}
