

#ifndef GUARD_WeightedGraph
#define GUARD_WeightedGraph

#define STAR INT_MAX

#include <Rcpp.h>
#include <vector>
#include <algorithm>
#include <random>
#include <stdexcept>

class Edge;

struct Vertex {
  int index;
  std::vector<Edge*> in_edges;
  std::vector<Edge*> out_edges;
};

struct DeltaRange {
  int low, up;
  DeltaRange() : low(INT_MIN), up(INT_MAX) {};
};

class Edge {
public: // interface
  Edge(Vertex* const h, Vertex* const t, int* const w, const bool f);
  void increment();
  void decrement();
  // getters and setters
  void reset() { visits_ = STAR; };
  const Vertex* tail() { return tail_; };
  const Vertex* head() { return head_; };
  const bool fixed() { return fixed_; };
  void weight(int);
  int pos() { return pos_; };
  int weight() { return *weight_; };
  int visits() { return visits_; };

private: // implementation
  Vertex* const head_, *const tail_;
  bool const fixed_;
  int* const weight_;
  int pos_, visits_;
  void add();
  void remove();
};

class WeightedGraph {
public:
  WeightedGraph(Rcpp::IntegerMatrix x, Rcpp::IntegerMatrix f);
  void sampleKernel(std::vector<Edge*> & vec);
  DeltaRange getDeltaRange(std::vector<Edge*> & vec);
  void updateWeights(std::vector<Edge*>& vec, int delta);
  void printData();
  void sampleStep();
  Rcpp::IntegerMatrix adjacency_matrix() { return adjacency_matrix_; };
  Edge** edges() { return edges_; };

private:
  std::vector<Vertex> vertices_;
  Edge** edges_;
  std::vector<Vertex*> initial_vertices_;
  Rcpp::IntegerMatrix adjacency_matrix_;
  std::default_random_engine generator_;
};

#endif
