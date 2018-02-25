
#include "FeasibleMatrix.h"

using namespace std;
using namespace Rcpp;

fm_vertex::fm_vertex():
  color(white),
  distance(0),
  predecessor(0){};

// adds an edge to adjacency matrix, while adjusting capacity,
void fm_graph::add_edge(unsigned int i, unsigned int j, unsigned int cap){
  adj[i].push_back(j);
  capacity[i][j] = cap;
}

fm_graph::fm_graph(IntegerVector r, IntegerVector c){

  // useful constants
  source=0;
  sink = r.size() + c.size() + 1;
  unsigned int nvertices = sink + 1;

  // allocating memory for data members
  vertices = vector<fm_vertex>(nvertices);
  adj = vector<vector<unsigned int> >(nvertices);
  flow = vector<vector<unsigned int> >(nvertices, vector<unsigned int>(nvertices,0));
  capacity = vector<vector<unsigned int> >(nvertices, vector<unsigned int>(nvertices,0));

  // initialise edges between source and rows
  for(unsigned int i=0;i!=r.size();++i){
    add_edge(source, i+1, r(i));
    add_edge(i+1, source, 0);
  }
  // initialise edges between columns and sink
  for(unsigned int j=0;j!=c.size();++j){
    add_edge(sink, r.size()+j+1, 0);
    add_edge(r.size()+j+1, sink, c(j));
  }
  // initialise all edges between rows and columns
  for(unsigned int i=0;i!=r.size();++i){
    for(unsigned int j=0;j!=c.size();++j){
      add_edge(i+1, r.size()+j+1, 1);
      add_edge(r.size()+j+1, i+1, 1);
    }
  }
}

bool fm_graph::find_path(){

  // initialise all vertices
  for(vector<fm_vertex>::iterator it=vertices.begin(); it!=vertices.end(); ++it){
    it->color = white;
    it->distance = 0;
    it->predecessor = 0;
  }

  vertices[source].color = gray;
  vertices[source].distance = 0;

  queue<unsigned int> Q;
  Q.push(source);

  while(Q.size()!=0 && vertices[sink].color == white){
    unsigned int u = Q.front();
    Q.pop();
    //loop through the adjacency list of vertex u
    for(vector<unsigned int>::iterator it=adj[u].begin(); it!=adj[u].end(); ++it){
      if(vertices[*it].color == white){
        // add additional condition checking if move possible in residual network
        if((int)capacity[u][*it] > (int)(flow[u][*it] - flow[*it][u])){
          vertices[*it].color = gray;
          vertices[*it].distance = vertices[u].distance+1;
          vertices[*it].predecessor = u;
          Q.push(*it);
          if(*it == sink) return true;
        }
      }
    }
    vertices[u].color = black;
  }
  // there is no augmenting path to the sink...
  return false;
}

// given a path, calculates the flow across that path
unsigned int fm_graph::calc_path_flow(){
  unsigned int v, f, edge_flow;
  v = sink;

  while(v!=source){
    edge_flow = capacity[vertices[v].predecessor][v] - flow[vertices[v].predecessor][v]
      + flow[v][vertices[v].predecessor];
    if(v==sink) f = edge_flow;
    else f = min(edge_flow, f);
    v = vertices[v].predecessor;
  }
  return f;
}

// method updates the flow in the graph according to the path found
void fm_graph::update_flow(unsigned int f){
  unsigned int v=sink;
  while(v!=source){
    flow[vertices[v].predecessor][v] += f;
    v=vertices[v].predecessor;
  }
}

IntegerMatrix fm_graph::construct_matrix(IntegerVector r, IntegerVector c){

  IntegerMatrix x(r.size(),c.size());

  for(unsigned int i=0; i!=r.size();++i)
    for(unsigned int j=0; j!=r.size();++j)
      x(i,j) = flow[i+1][r.size()+j+1] - flow[r.size()+j+1][i+1];

  // validate the matrix
  vector<int> rwrong, cwrong;
  for(unsigned int i=0; i!=r.size();++i){
    int total = 0;
    for(unsigned int j=0; j!=c.size();++j){
      total += x(i,j);
    }
    if(total!=r(i))
      rwrong.push_back(i+1);
  }

  for(unsigned int j=0; j!=c.size();++j){
    int total = 0;
    for(unsigned int i=0; i!=r.size();++i){
      total += x(i,j);
    }
    if(total!=c(j))
      cwrong.push_back(j+1);
  }

  if(rwrong.size()>0 || cwrong.size()>0){
    std::stringstream Message;
    Message<<"Could not construct matrix with given margins and fixed elements. Please ensure such a matrix exists.";
    throw Rcpp::exception(Message.str().c_str());
  }

  return x;
}
