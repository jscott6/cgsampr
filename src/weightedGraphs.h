

#ifndef GUARD_weightedGraph
#define GUARD_weightedGraph

#include <Rcpp.h>
#include <vector>
#include <algorithm>

using namespace std;
using namespace Rcpp;


class edge;

struct vertex{
  vector<edge*> p_in_edges;
  vector<const edge*> p_poss_out_edges;
};

class edge{
private: // implementation
  const vertex* p_head, *p_tail;
  const bool fixed;
  // take weight by reference to avoid reconstructing adj matrix for each sample
  const int * p_weight;
  edge ** p_p_tail_p_in_edges;

  void add();
  void remove();

public: // interface
  edge(const vertex* ph,const vertex* pt, const int* pw, const bool f):
    p_head(ph),
    p_tail(pt),
    fixed(f),
    p_weight(pw){};
  void set_weight();
  int weight(){return *p_weight;};
  const bool is_fixed(){return fixed;};
};

class weightedGraph{

private:
  vector<vertex> vertices;
  vector<vector<edge> > edges;
  Rcpp::IntegerMatrix adj_matrix;

public:
  weightedGraph(Rcpp::IntegerMatrix x0, Rcpp::IntegerMatrix f);
  void print_data();

};


#endif
