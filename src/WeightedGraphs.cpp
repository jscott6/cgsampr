
#include "weightedGraphs.h"


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

  for(int i=0; i!=nrow+ncol;++i)
    vertices[i].index = i+1;

  return;
}


void printVertexData(vertex &v){

  Rcout << "In Edges" << endl;
  Rcout << "Tail: ";
  for(int i=0; i!=v.p_in_edges.size();++i){
    Rcout << setw(2) << v.p_in_edges[i]->tail()->index << " ";
  }
  Rcout << endl;
  Rcout << "Head: ";
  for(int i=0; i!=v.p_in_edges.size();++i){
    Rcout << setw(2) << v.p_in_edges[i]->head()->index << " ";
  }
  Rcout << endl;

  Rcout << "Possible Out Edges" << endl;
  Rcout << "Tail: ";
  for(int i=0; i!=v.p_poss_out_edges.size();++i){
    Rcout << setw(2) << v.p_poss_out_edges[i]->tail()->index << " ";
  }
  Rcout << endl;
  Rcout << "Head: ";
  for(int i=0; i!=v.p_poss_out_edges.size();++i){
      Rcout << setw(2) << v.p_poss_out_edges[i]->head()->index << " ";
  }
  Rcout << endl;
  return;
}


void weightedGraph::printData(){

  int nrow = adj_matrix.nrow(), ncol = adj_matrix.ncol();
  for(int i=0;i!=nrow;++i){
    for(int j=0;j!=ncol;++j)
      Rcout << edges[i][j].weight() << " ";
    Rcout << endl;
  }
  Rcout << endl;

  for(int i=0; i!=vertices.size(); ++i){
    Rcout << "Vertex " << vertices[i].index << endl;
    printVertexData(vertices[i]);
    Rcout << endl;
  }
  return;
}
