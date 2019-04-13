
#include "graph.h"
#include "AuxiliaryFunctions.h"
#include <utility>

using namespace Unweighted::Directed;
using namespace Unweighted;
using namespace std;
using namespace Rcpp;

Edge::Edge(Vertex *const head, Vertex *const tail, int *const weight, const bool fixed)
    : Base::Edge<Vertex>(head, tail, weight, fixed)
{
  if (*weight_ == 1 && !fixed_)
    head_->in_edges.push_back(this);
  else if (!fixed_)
    tail_->not_out_edges.push_back(this);
  return;
}

vector<Vertex*> init_initial_vertices(IM adjacency_matrix, IM fixed, vector<Vertex>& vertices)
{
    vector<Vertex*> initial_vertices;
    auto const nrow = adjacency_matrix.nrow(), ncol = adjacency_matrix.ncol();
    // initialise initial_vertices_
    for (int i = 0; i != nrow + ncol; ++i)
        if (vertices[i].in_edges.size() != 0)
            initial_vertices.push_back(&vertices[i]);
    if (initial_vertices.size() == 0)
        throw invalid_argument("Matrix fully determined by specification");
    return initial_vertices;
}

Graph::Graph(IM adjacency_matrix, IM fixed)
    : ::Directed::Graph<Vertex, Edge>(adjacency_matrix,fixed, true),
        initial_vertices_(init_initial_vertices(adjacency_matrix_, fixed_, vertices_))
{
}


void Graph::printData() const
{
    for (const auto &v : vertices_)
    {
        Rcout << &v << endl;
        Rcout << (&v)->index + 1 << endl;
        Rcout << "Vertex: " << v.index + 1 << endl;
        Rcout << "----------------------" << endl;
        Rcout << "In Edges: " << endl;
        for(const auto &e : v.in_edges)
            Rcout << e->tail_->index + 1 << "->" << e->head_->index + 1 << endl;
        Rcout << endl;
        Rcout << "Not Out Edges: " << endl;
        for(const auto &e: v.not_out_edges)
            Rcout << e->tail_->index + 1 << "->" << e->head_->index + 1 << endl;
        Rcout << endl;
    }
}

void Graph::updateAdjacencyMatrix()
{
    for(const auto & v : vertices_)
    {
        for(const auto & e : v.in_edges)
            e->weight(1);
        for(const auto & e : v.not_out_edges)
            e->weight(0);
    }
}


// overloading swap to use pointers
template<class T>
void swap(T* a, T* b)
{
    T temp = *a;
    *a = *b;
    *b = temp;
}

void Graph::sampleStep()
{
    Vertex *u0 = sampleFromVector(initial_vertices_, generator_);
    Edge ** pe0 = sampleReferenceFromVector(u0->in_edges, generator_);
    Edge * e = *pe0;
    Edge ** pe;
    while(true)
    {
        pe = sampleReferenceFromVector(e->tail_->not_out_edges, generator_);
        swap(&e, pe);
        if (e->head_ == u0)
            break;
        pe = sampleReferenceFromVector(e->head_->in_edges, generator_);
        swap(&e, pe);
    }
    swap(&e, pe0);
}



