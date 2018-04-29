
#include "weightedGraphs.h"
#include "AuxiliaryFunctions.h"


weightedGraph::weightedGraph(Rcpp::IntegerMatrix x0, Rcpp::IntegerMatrix f){

  adj_matrix = clone(x0);
  int nrow = x0.nrow(), ncol = x0.ncol();

  // assuming biadjacency matrix
  vertices = vector<vertex>(nrow + ncol);

  // initialise edges
  for(int i=0;i!=nrow;++i){
    vector<edge> es;
    for(int j=0;j!=ncol;++j){
      edge e(&vertices[nrow+j],&vertices[i], &adj_matrix(i,j), f(i,j));
      es.push_back(e);
    }
    edges.push_back(es);
  }

  // initialise vertices in left partition (i.e. add possible out edges)
  for(int i=0; i!=nrow; ++i)
    for(int j=0; j!=ncol; ++j)
      if(!edges[i][j].is_fixed())
        vertices[i].p_poss_out_edges.push_back(&edges[i][j]);

  // initialise vertices in right partition (i.e. add all in edges)
  for(int j=0; j!=ncol; ++j)
    for(int i=0; i!=nrow; ++i)
      if(!edges[i][j].is_fixed() && (edges[i][j].weight()>0))
        vertices[nrow+j].p_in_edges.push_back(&edges[i][j]);

  for(int i=0; i!=nrow+ncol;++i){
    vertices[i].index = i;
    if(vertices[i].p_in_edges.size()!=0)
      init_vertices.push_back(&vertices[i]);
  }

  return;
}


void weightedGraph::sampleStep(){

  vertex* u0 = sampleFromVector(init_vertices);
  edge* e = sampleFromVector(u0->p_in_edges);
  Rcout << e->tail()->index + 1 << "->" << e->head()->index + 1 << endl;
  return;
}


/*

// adds edge to in 'in edges' of head vertex
// assumes edge is NOT already referenced in vertex data
// will only be called as part of set_weight
void edge::add(){
  p_head->p_in_edges.push_back(this);
  p_p_head_p_in_edges = p_head->p_in_edges->back();
}

// removes edge from 'in edges' of a vertex
// assumes edge is referenced in vertex at location p_p_head_p_in_edges
// will only be called as part of set_weight
void edge::remove(){
    p_head->p_in_edges->back()->p_p_head_p_in_edges = p_p_head_p_in_edges;
    swap(p_p_head_p_in_edges, p_head->p_in_edges->back());
    p_head->p_in_edges.pop_back();
}


*/

void weightedGraph::printData(){

  int nrow = adj_matrix.nrow(), ncol = adj_matrix.ncol();
  for(int i=0;i!=nrow;++i){
    for(int j=0;j!=ncol;++j)
      Rcout << edges[i][j].weight() << " ";
    Rcout << endl;
  }
  Rcout << endl;

  for(int i=0; i!=vertices.size(); ++i){
    Rcout << "------------------------" << endl;
    Rcout << "VERTEX " << vertices[i].index+1 << endl;
    Rcout << "------------------------" << endl;
    Rcout << endl;
    printVertexData(vertices[i]);
    Rcout << endl;
  }
  return;
}
