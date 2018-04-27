
#include <iostream>
#include <vector>
using namespace std;


struct vertex{
  vector<*edge> p_in_edges;
  const vector<*edge> p_poss_out_edges;
};

class edge{

private: // implementation
  const vertex* p_head, *p_tail;
  // take weight by reference to avoid reconstructing adj matrix for each sample
  const unsigned int * p_weight;
  edge ** p_p_tail_p_in_edges;
  void add();
  void remove();

public: // interface
  edge(const vertex* ph,const vertex* pt, const unsigned_int* pw):
    p_head(ph),
    p_tail(pt),
    p_weight(pw){}
  void set_weight();
};


class weightedGraph{

private:
  vector<vertex> vertices;
  vector<vector<edge> > edges;
  vector<vector<signed int> > adj_matrix;

public:
  weightedGraph()

};
