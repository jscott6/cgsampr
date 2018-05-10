
#include "AuxiliaryFunctions.h"
using namespace std;
using namespace Rcpp;

// prints a 2-d array to output stream
void printMatrix(IntegerMatrix vec) {
  int n = vec.nrow();
  Rcout << endl;
  for (int i = 0; i < n; i++) {
    int m = vec.ncol();
    for (int j = 0; j < m; j++){
      Rcout << setprecision(2) << setw(4);
      Rcout << vec(i,j) << " ";
    }
    Rcout << endl;
  }
  Rcout << endl;
}

void printVertexData(Weighted::Vertex& v) {
  Rcout << "------------------------" << endl;
  Rcout << "VERTEX " << v.index + 1 << endl;
  Rcout << "------------------------" << endl;
  Rcout << endl;
  Rcout <<  setw(8) << "In Edges" << endl;
  Rcout << "------------" << endl;
  Rcout <<  setw(8) << "Tail: ";
  for (int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->tail_->index + 1 << " ";
  Rcout << endl;
  Rcout <<  setw(8) << "Head: ";
  for(int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->head_->index + 1 << " ";
  Rcout << endl;
  Rcout <<  setw(8) << "Weight: ";
  for(int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->weight() << " ";
  Rcout << endl;
  Rcout << endl;
  Rcout << setw(8) << "Possible Out Edges" << endl;
  Rcout << "------------" << endl;
  Rcout <<  setw(8) << "Tail: ";
  for (int i = 0; i != v.out_edges.size(); ++i) {
    Rcout << setw(2) << v.out_edges[i]->tail_->index + 1<< " ";
  }
  Rcout << endl;
  Rcout <<  setw(8) << "Head: ";
  for (int i = 0; i != v.out_edges.size(); ++i) {
      Rcout << setw(2) << v.out_edges[i]->head_->index + 1<< " ";
  }
  Rcout << endl;
  Rcout <<  setw(8) << "Weight: ";
  for (int i=0; i != v.out_edges.size(); ++i) {
    Rcout << setw(2) << v.out_edges[i]->weight() << " ";
  }
  Rcout << endl;
  return;
}
