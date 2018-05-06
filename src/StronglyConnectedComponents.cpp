
#include "StronglyConnectedComponents.h"
using namespace Rcpp;
using namespace std;
using namespace StronglyConnectedComponents;

// constructs required directed bipartite Graph directly from adjacency matrix
Graph::Graph(IntegerMatrix adjacency_matrix, IntegerMatrix fixed){
  for (unsigned int i = 0; i != adjacency_matrix.nrow(); ++i) {
    Vertex v(i);
    for (unsigned int j = 0; j != adjacency_matrix.ncol(); ++j) {
      if (adjacency_matrix(i,j) == 0 && fixed(i,j) == 0)
        v.neighbours.push_back(adjacency_matrix.nrow() + j);
    }
    vertices.push_back(v);
  }
  for (unsigned int j = 0; j != adjacency_matrix.ncol(); ++j) {
    Vertex v(adjacency_matrix.nrow() + j);
    for (unsigned int i = 0; i != adjacency_matrix.nrow(); ++i) {
      if(adjacency_matrix(i,j) == 1 && fixed(i,j) == 0)
        v.neighbours.push_back(i);
    }
    vertices.push_back(v);
  }
}

unsigned int Graph::DFSVisit(unsigned int i, unsigned int time, unsigned int tree) {
  time = time + 1;
  vertices[i].discovery = time;
  vertices[i].color = gray;
  for (vector<int>::iterator it = vertices[i].neighbours.begin();
    it != vertices[i].neighbours.end(); ++it) {
      if (vertices[*it].color==white) {
        vertices[*it].predecessor = i;
        time = DFSVisit(*it, time, tree);
      }
    }
    vertices[i].color = black;
    time = time + 1;
    vertices[i].finished = time;
    vertices[i].tree = tree;
    return time;
}

bool compareVertexFinishTimes(Vertex& a, Vertex& b) {
  return (a.finished > b.finished);
}

void Graph::DFS() {
  // initialise member data of each vertex in the Graph
  vector<Vertex> sorted_vertices = vertices;
  sort(sorted_vertices.begin(), sorted_vertices.end(), compareVertexFinishTimes);

  for (auto &v: vertices) {
    v.color = white;
    v.predecessor = 0;
  }

  unsigned int time=0, tree=0;
  // for each Vertex, if white visit all edges via DFSVisit
  for (auto &v: vertices) {
    if (vertices[v.name].color == white) {
      ++tree;
      time = DFSVisit(v.name, time, tree);
    }
  }
  return;
}

//method computes transpose of the Graph. i.e. each edge switches direction
void Graph::transpose() {
  vector<vector<int> > tneighbours(vertices.size(), vector<int>());
  const unsigned int N = vertices.size();
  for (int i = 0; i != N; ++i)
  for (auto& v: vertices)
    for (auto& w: v.neighbours)
      if (i == w) tneighbours[i].push_back(v.name);
  // replace Graph neighbours
  for (int i=0; i!=N; ++i) vertices[i].neighbours = tneighbours[i];
}

void Graph::stronglyConnectedComponents(){
  DFS();
  transpose();
  DFS();
}

// given an input matrix F, computes additional forced fixed_ values
IntegerMatrix Graph::fixed_values(IntegerMatrix fixed){
  const unsigned int I = fixed.nrow();
  const unsigned int J = fixed.ncol();

  stronglyConnectedComponents();

  for (int i = 0; i != I; ++i)
    for (int j = 0; j != J; ++j)
      if (vertices[i].tree != vertices[I+j].tree) fixed(i,j) = 1;
  return fixed;
}
