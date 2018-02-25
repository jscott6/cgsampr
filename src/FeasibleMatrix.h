
#ifndef GUARD_FeasibleMatrix
#define GUARD_FeasibleMatrix

#include <Rcpp.h>
#include <vector>
#include <queue>

#ifndef GUARD_vertexcolor
#define GUARD_vertexcolor
enum vertexcolor{white, gray, black};
#endif

class fm_vertex{
  friend class fm_graph;
  unsigned int distance;
  vertexcolor color;
  unsigned int predecessor;

public:
  fm_vertex();
};

class fm_graph{
  std::vector<fm_vertex> vertices;
  std::vector<std::vector<unsigned int> > adj, flow, capacity;
  unsigned int source, sink;
public:
  fm_graph(Rcpp::IntegerVector, Rcpp::IntegerVector);
  void add_edge(unsigned int, unsigned int, unsigned int);
  bool find_path();
  unsigned int calc_path_flow();
  void update_flow(unsigned int);
  Rcpp::IntegerMatrix construct_matrix(Rcpp::IntegerVector, Rcpp::IntegerVector);
};

#endif
