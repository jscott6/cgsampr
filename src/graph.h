#ifndef GUARD_graph
#define GUARD_graph

#include <Rcpp.h>
#include <vector>
#include <algorithm>
#include <random>
#include <string>
#include <iomanip>

// Too many data structures for one classs. Refactor into classes in
// different namespaces, with each one having only its correct operations.
// i.e. nested namespace. Have Unweighted as outermost namespace, then
// nest classes in each one for each method.

namespace UnWeighted {
  struct Edge{
    int head;
    int tail;
  };
  class Graph {
  public:
    Graph(Rcpp::IntegerMatrix adjacency_matrix_, Rcpp::IntegerMatrix fixed_,
       bool digraph = FALSE);
    Graph(Rcpp::IntegerVector in_degrees, Rcpp::IntegerVector out_degrees,
       Rcpp::IntegerMatrix f, bool digraph = FALSE);
    Rcpp::List sample(std::string method, int nsamples, int thin, int burnin);
    void printData();
    void printEdgeList();
    void printStubList();
    Rcpp::IntegerMatrix adjacency_matrix() { return adjacency_matrix_; };
    Rcpp::IntegerMatrix fixed() { return fixed_; };

  private:
    std::vector<int> one_nums_, zero_nums_, in_stubs_, out_stubs_;
    bool directed_;
    std::vector<UnWeighted::Edge> edge_list_;
    std::vector<std::vector<int> > ones_, zeros_;
    std::discrete_distribution<int> one_dist_;
    std::vector<std::uniform_int_distribution<int> > one_sampler_, zero_sampler_;
    std::vector<std::vector<std::vector<int> > > tracking_;
    std::default_random_engine generator_;
    int nrow_, ncol_, nstubs_;
    signed int rejected_;
    Rcpp::IntegerMatrix adjacency_matrix_;
    Rcpp::IntegerMatrix fixed_;
    void updateAdjacencyMatrix();
    void init(Rcpp::IntegerMatrix adjacency_matrix_, Rcpp::IntegerMatrix fixed_);
    void switchStep();
    void DGStep();
    void SGStep();
    void matchingStep();
    void updateData();
  };
}



#endif
