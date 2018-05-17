

#ifndef GUARD_StronglyConnectedComponents
#define GUARD_StronglyConnectedComponents

#include <Rcpp.h>
#include <vector>

namespace StronglyConnectedComponents
{

enum VertexColor
{
  white,
  gray,
  black
};

struct Vertex
{
  unsigned int name;
  VertexColor color;
  unsigned int predecessor;
  unsigned int discovery;
  unsigned int finished;
  unsigned int tree;
  std::vector<int> neighbours;
  Vertex(int i) : name(i),
                  color(white),
                  predecessor(0),
                  discovery(0),
                  finished(0),
                  neighbours(std::vector<int>()){};
};

class Graph
{
public:
  Graph(Rcpp::IntegerMatrix, Rcpp::IntegerMatrix);
  Rcpp::IntegerMatrix fixed_values(Rcpp::IntegerMatrix);

private:
  unsigned int DFSVisit(unsigned int, unsigned int, unsigned int);
  void DFS();
  void transpose();
  void stronglyConnectedComponents();
  // container for the graph vertices
  std::vector<Vertex> vertices;
};
} // namespace StronglyConnectedComponents

#endif
