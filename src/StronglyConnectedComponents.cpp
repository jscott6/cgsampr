
#include "StronglyConnectedComponents.h"

// ok to use namespace std as library is for R not C++
using namespace Rcpp;
using namespace std;


scc_vertex::scc_vertex(int i):
  name(i),
  color(white),
  predecessor(0),
  discovery(0),
  finished(0){
  neighbours = vector<int>();
}

// constructs required directed bipartite graph directly from binary matrix
scc_graph::scc_graph(IntegerMatrix x, IntegerMatrix f){

  for(unsigned int i=0; i!=x.nrow(); ++i){
    scc_vertex v(i);
    for(unsigned int j=0; j!=x.ncol();++j){
      if(x(i,j)==0 && f(i,j)==0)
        v.neighbours.push_back(x.nrow()+j);
    }
    vertices.push_back(v);
  }

  for(unsigned int j=0; j!=x.ncol(); ++j){
    scc_vertex v(x.nrow()+j);
    for(unsigned int i=0; i!=x.nrow();++i){
      if(x(i,j)==1 && f(i,j)==0)
        v.neighbours.push_back(i);
    }
    vertices.push_back(v);
  }
}



unsigned int scc_graph::DFS_Visit(unsigned int i, unsigned int time, unsigned int tree){
  time = time+1;
  vertices[i].discovery = time;
  vertices[i].color = gray;
  for(vector<int>::iterator it=vertices[i].neighbours.begin();
    it!=vertices[i].neighbours.end(); ++it){
      if(vertices[*it].color==white){
        vertices[*it].predecessor = i;
        time = DFS_Visit(*it,time, tree);
      }
    }
    vertices[i].color = black;
    time = time+1;
    vertices[i].finished = time;
    vertices[i].tree = tree;
    return time;
}

bool compare_vertex_ftimes(scc_vertex& a, scc_vertex& b){
  return (a.finished > b.finished);
}

void scc_graph::DFS(){

  // initialise member data of each vertex in the graph

  vector<scc_vertex> sorted_vertices = vertices;
  sort(sorted_vertices.begin(), sorted_vertices.end(), compare_vertex_ftimes);

  for(vector<scc_vertex>::iterator iter = vertices.begin(); iter!=vertices.end(); ++iter){
    iter->color = white;
    iter->predecessor = 0;
  }

  unsigned int time=0;
  unsigned int tree=0;
  // for each scc_vertex, if white visit all edges via DFS_Visit
  for(vector<scc_vertex>::iterator iter=sorted_vertices.begin();iter!=sorted_vertices.end();++iter){
    if(vertices[iter->name].color==white){
      ++tree;
      time = DFS_Visit(iter->name, time, tree);
    }

  }

  return;
}

/*
method computes transpose of the graph. i.e. each edge switches direction
*/
void scc_graph::transpose(){

  vector<vector<int> > tneighbours(vertices.size(),vector<int>());
  const unsigned int N = vertices.size();
  for(int i=0; i!=N; ++i)
    for(vector<scc_vertex>::iterator it=vertices.begin(); it!=vertices.end(); ++it)
      for(vector<int>::iterator nit=(it->neighbours).begin(); nit!=(it->neighbours).end();++nit)
        // if the current vertex i is a neighbour of *it then in transpose
        // graph *it is a neighbour if *vit...
        if(i == *nit) tneighbours[i].push_back(it->name);

  // replace graph neighbours
  for(int i=0; i!=N; ++i) vertices[i].neighbours = tneighbours[i];

}

void scc_graph::strongly_connected_components(){
  DFS();
  transpose();
  DFS();
}


// given an input matrix F, computes additional forced fixed values
IntegerMatrix scc_graph::fixed_values(IntegerMatrix f){

  strongly_connected_components();

  const unsigned int I = f.nrow();
  const unsigned int J = f.ncol();

  for(int i=0; i!=I;++i)
    for(int j=0;j!=J;++j)
      if(vertices[i].tree != vertices[I+j].tree) f(i,j)=1;

  return f;
}
