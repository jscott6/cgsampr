

#ifndef GUARD_weightedGraph
#define GUARD_weightedGraph

#include <Rcpp.h>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;
using namespace Rcpp;


class edge;

struct vertex{
  int index;
  vector<edge*> p_in_edges;
  vector<edge*> p_poss_out_edges;
};

class edge{
private: // implementation
  const vertex* p_head, *p_tail;
  const bool fixed;
  // take weight by reference to avoid reconstructing adj matrix for each sample
  const int * p_weight;
  edge ** p_p_head_p_in_edges;

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
  const vertex* tail(){return p_tail;};
  const vertex* head(){return p_head;};
  const bool is_fixed(){return fixed;};
};

class weightedGraph{

private:
  vector<vertex> vertices;
  vector<vector<edge> > edges;
  vector<vertex*> init_vertices;
  Rcpp::IntegerMatrix adj_matrix;
  default_random_engine generator;


public:
  weightedGraph(Rcpp::IntegerMatrix x0, Rcpp::IntegerMatrix f);
  void printData();
  void sampleStep();
};


#endif
