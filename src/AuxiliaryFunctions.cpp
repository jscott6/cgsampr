
#include "AuxiliaryFunctions.h"
using namespace std;
using namespace Rcpp;
using IV = Rcpp::IntegerVector;
using IM = Rcpp::IntegerMatrix;

// prints a 2-d array to output stream
void printMatrix(const IM vec)
{
  int n = vec.nrow();
  Rcout << endl;
  for (int i = 0; i < n; i++)
  {
    int m = vec.ncol();
    for (int j = 0; j < m; j++)
    {
      Rcout << setprecision(2) << setw(4);
      Rcout << vec(i, j) << " ";
    }
    Rcout << endl;
  }
  Rcout << endl;
}

// This if for debugging purposes

void printEdgeData(const Weighted::Edge &e)
{
  Rcout << "Edge: " << e.tail_->index + 1 << "->" << e.head_->index + 1 << endl;
  Rcout << "Weight: " << e.weight() << endl;
  Rcout << endl;
}

void printEdgeData(const Unweighted::Edge &e)
{
  Rcout << "Edge: " << e.tail_->index + 1 << "->" << e.head_->index + 1 << endl;
  Rcout << "Weight: " << e.weight() << endl;
  Rcout << endl;
}


void printVertexData(const Weighted::Vertex &v)
{
  Rcout << "------------------------" << endl;
  Rcout << "VERTEX " << v.index + 1 << endl;
  Rcout << "------------------------" << endl;
  Rcout << endl;
  Rcout << setw(8) << "In Edges" << endl;
  Rcout << "------------" << endl;
  Rcout << setw(8) << "Tail: ";
  for (int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->tail_->index + 1 << " ";
  Rcout << endl;
  Rcout << setw(8) << "Head: ";
  for (int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->head_->index + 1 << " ";
  Rcout << endl;
  Rcout << setw(8) << "Weight: ";
  for (int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->weight() << " ";
  Rcout << endl;
  Rcout << endl;
  Rcout << setw(8) << "Possible Out Edges" << endl;
  Rcout << "------------" << endl;
  Rcout << setw(8) << "Tail: ";
  for (int i = 0; i != v.out_edges.size(); ++i)
  {
    Rcout << setw(2) << v.out_edges[i]->tail_->index + 1 << " ";
  }
  Rcout << endl;
  Rcout << setw(8) << "Head: ";
  for (int i = 0; i != v.out_edges.size(); ++i)
  {
    Rcout << setw(2) << v.out_edges[i]->head_->index + 1 << " ";
  }
  Rcout << endl;
  Rcout << setw(8) << "Weight: ";
  for (int i = 0; i != v.out_edges.size(); ++i)
  {
    Rcout << setw(2) << v.out_edges[i]->weight() << " ";
  }
  Rcout << endl;
  Rcout << endl;
  Rcout << "n_int:" << v.n_int << endl;
  Rcout << "n_up:" << v.n_up << endl;
  Rcout << "n_low:" << v.n_low << endl;
  return;
}



void printVertexData(const Unweighted::Vertex &v)
{
  Rcout << "------------------------" << endl;
  Rcout << "VERTEX " << v.index + 1 << endl;
  Rcout << "------------------------" << endl;
  Rcout << endl;
  Rcout << setw(8) << "In Edges" << endl;
  Rcout << "------------" << endl;
  Rcout << setw(8) << "Tail: ";
  for (int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->tail_->index + 1 << " ";
  Rcout << endl;
  Rcout << setw(8) << "Head: ";
  for (int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->head_->index + 1 << " ";
  Rcout << endl;
  Rcout << setw(8) << "Weight: ";
  for (int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->weight() << " ";
  Rcout << endl;
  Rcout << endl;
  Rcout << setw(8) << "Not Out Edges" << endl;
  Rcout << "------------" << endl;
  Rcout << setw(8) << "Tail: ";
  for (int i = 0; i != v.not_out_edges.size(); ++i)
  {
    Rcout << setw(2) << v.not_out_edges[i]->tail_->index + 1 << " ";
  }
  Rcout << endl;
  Rcout << setw(8) << "Head: ";
  for (int i = 0; i != v.not_out_edges.size(); ++i)
  {
    Rcout << setw(2) << v.not_out_edges[i]->head_->index + 1 << " ";
  }
  Rcout << endl;
  Rcout << setw(8) << "Weight: ";
  for (int i = 0; i != v.not_out_edges.size(); ++i)
  {
    Rcout << setw(2) << v.not_out_edges[i]->weight() << " ";
  }
  Rcout << endl;
  Rcout << endl;
  return;
}

// [[Rcpp::export]]
IM constructGraph(IV out_degree, IV in_degree, IM fixed, bool weighted)
{
  checkInputs(in_degree, out_degree, fixed);
  bool sinkfound = true;
  ConstructGraph::Graph net(out_degree, in_degree, fixed, weighted);
  // adjust flow until no path remains in the residual Graph
  while (sinkfound) {
    sinkfound = net.findPath();
    net.updateFlow(net.calcPathFlow());
  }
  IM adjacency_matrix = net.constructWeightMatrix();
  if(is_true(all(rowSums(adjacency_matrix) == out_degree)) &&
     is_true(all(colSums(adjacency_matrix) == in_degree)))
    return adjacency_matrix;
  else
    throw invalid_argument("No graph is compatible with inputs provided");
}

void checkInputs(IV in_degree, IV out_degree, IM fixed) {
  if(in_degree.size() != fixed.nrow() || out_degree.size() != fixed.ncol())
    throw invalid_argument("Degree vectors and fixed matrix of incompatible dimensions");
  if(min(out_degree) < 0 || min(in_degree) < 0)
    throw invalid_argument("Degree vectors must be non-negative");
  if(min(fixed) < 0 || max(fixed) > 1)
    throw invalid_argument("All entries of fixed matrix must be binary valued");
}






