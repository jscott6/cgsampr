
#include "weighted_graph.h"

using namespace std;
using namespace Rcpp;
using namespace Weighted::Directed::DG;
using namespace Weighted;
using IM = IntegerMatrix;
using IV = IntegerVector;


// samples delta uniformly from provided range
int sampleDelta(const DeltaRange &dr, default_random_engine &gen)
{
    uniform_int_distribution<int> dist(dr.low, dr.up);
    return dist(gen);
}

// no additional data structures required
Graph::Graph(IM adjacency_matrix, IM fixed)
    : ::Directed::Graph<Vertex, Edge>(adjacency_matrix, init_fixed(adjacency_matrix, fixed), false)
{
}

// populates vector passed by reference with pointers to updates edges
int Graph::sampleKernel(vector<Edge *> &vec)
{
    int nrow = adjacency_matrix_.nrow();
    int ncol = adjacency_matrix_.ncol();
    uniform_int_distribution<int> row_dist(0, nrow-1);
    uniform_int_distribution<int> col_dist(0, ncol-1);
    vector<int> I, J;
    I.push_back(row_dist(generator_));
    I.push_back(row_dist(generator_));
    J.push_back(col_dist(generator_));
    J.push_back(col_dist(generator_));

    if(I[0] == I[1] || J[0] == J[1])
        return 1;
    // populate vector
    vec.push_back(&edges_[I[0]][J[0]]);
    vec.push_back(&edges_[I[0]][J[1]]);
    vec.push_back(&edges_[I[1]][J[1]]);
    vec.push_back(&edges_[I[1]][J[0]]);

    // if any edge is fixed, reject
    for(const auto & e: vec)
        if(e->fixed())
            return 1;
    return 0;
}

// computes support for conditional distribution of Delta
DeltaRange Graph::getDeltaRange(vector<Edge *> &vec)
{
    DeltaRange dr;
    for (int i = 0; i != vec.size(); ++i)
    {
        if(i % 2)
            dr.up = min(dr.up, vec[i]->weight());
        else
            dr.low = max(dr.low, -(vec[i]->weight()));
    }
    return dr;
}

// Once delta is sampled, update graphs along kernel
void Graph::updateWeights(vector<Edge *> &vec, int delta)
{
    for(int i = 0; i != vec.size(); ++i)
    {
        if(i % 2)
            vec[i]->weight(vec[i]->weight() - delta);
        else
            vec[i]->weight(vec[i]->weight() + delta);
    }
}

// performs one full sampling step
void Graph::sampleStep()
{
    vector<Edge *> cycle;
    int discard = sampleKernel(cycle);
    if(discard)
        return;
    DeltaRange dr = getDeltaRange(cycle);
    int delta = sampleDelta(dr, generator_);
    updateWeights(cycle, delta);
    return;
}


void Graph::updateAdjacencyMatrix()
{
}

