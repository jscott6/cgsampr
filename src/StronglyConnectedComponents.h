

#ifndef GUARD_StronglyConnectedComponents
#define GUARD_StronglyConnectedComponents

#include <Rcpp.h>
#include <vector>

#ifndef GUARD_vertexcolor
#define GUARD_vertexcolor
enum vertexcolor{white, gray, black};
#endif

class scc_vertex{
  friend class scc_graph;
  friend bool compare_vertex_ftimes(scc_vertex&, scc_vertex&);
  unsigned int name;
  vertexcolor color;
  unsigned int predecessor;
  unsigned int discovery;
  unsigned int finished;
  unsigned int tree;
  std::vector<int> neighbours;
public:
  scc_vertex(int );
};

class scc_graph{
private:
  unsigned int DFS_Visit(unsigned int, unsigned int, unsigned int);
  void DFS();
  void transpose();
  void strongly_connected_components();
  // container for the graph vertices
  std::vector<scc_vertex> vertices;
public:
  scc_graph(Rcpp::IntegerMatrix, Rcpp::IntegerMatrix);
  Rcpp::IntegerMatrix fixed_values(Rcpp::IntegerMatrix);
};

#endif
