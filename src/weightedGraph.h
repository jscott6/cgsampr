

#ifndef GUARD_weightedGraph
#define GUARD_weightedGraph

#define STAR INT_MAX

#include <Rcpp.h>
#include <vector>
#include <algorithm>
#include <random>
#include <stdexcept>

class edge;

struct vertex{
  int index;
  std::vector<edge*> p_in_edges;
  std::vector<edge*> p_poss_out_edges;
};

struct deltaRange{
  int low, up;
  deltaRange(): low(INT_MIN), up(INT_MAX){};
};


class edge{
private: // implementation
  vertex* const p_head, *p_tail;
  bool const fixed;
  // take weight by reference to avoid reconstructing adj matrix for each sample
  int * const p_weight;
  int  pos;
  int m_visits;

  void add();
  void remove();

public: // interface
  edge(vertex* const ph,vertex* const pt, int* const pw, const bool f):
    p_head(ph),
    p_tail(pt),
    fixed(f),
    p_weight(pw),
    pos(-1),
    m_visits(STAR){};
  void set_weight(int);
  void set_pos(int pp){pos=pp;};
  int weight(){return *p_weight;};
  int visits(){return m_visits;};
  void increment();
  void decrement();
  void reset();
  const vertex* tail(){return p_tail;};
  const vertex* head(){return p_head;};
  const bool is_fixed(){return fixed;};
  int get_pos(){return pos;};
};



class weightedGraph{

private:
  std::vector<vertex> vertices;
  std::vector<std::vector<edge> > edges;
  std::vector<vertex*> init_vertices;
  Rcpp::IntegerMatrix adj_matrix;
  std::default_random_engine generator;
  void sampleKernel(std::vector<edge*> & vec);
  deltaRange getDeltaRange(std::vector<edge*> & vec);
  void updateWeights(std::vector<edge*>& vec, int delta);


public:
  weightedGraph(Rcpp::IntegerMatrix x0, Rcpp::IntegerMatrix f);
  void printData();
  void sampleStep();
  Rcpp::IntegerMatrix get_adj_matrix(){return adj_matrix;};
};


#endif
