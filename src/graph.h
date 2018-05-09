#ifndef GUARD_graph
#define GUARD_graph

#include <Rcpp.h>
#include <vector>
#include <random>

#define pure = 0;
#define IM = Rcpp::IntegerMatrix
#define IV = Rcpp::IntegerVector

namespace UnWeighted {

  class AbstractGraph {
  public:
    GraphInterface(IM adjacency_matrix, IM fixed, bool digraph = FALSE);
    GraphInterface(IV in_degrees, IV out_degrees, IM f, bool digraph = FALSE);
    Rcpp::List sample(int nsamples = 10000, int thin = 10, int burnin = 5000);
    void summary();
    // getter methods
    IM adjacency_matrix() { return adjacency_matrix_; };
    IM fixed() { return fixed_; };
  private:
    bool directed_;
    std::default_random_engine generator_;
    IM adjacency_matrix_, fixed_;
    virtual void updateAdjacencyMatrix() pure;
    virtual void sampleStep() pure;
  };

  namespace SG {
    namespace Directed {

      struct Edge {
        Vertex *tail, *head;
        int fixed;
        Edge(Vertex* t, Vertex* h, Vertex* f)
          : tail(t), head(h), fixed(f) {}
      };

      struct Vertex {
        int index;
        std::vector<Edge*> in_edges, not_out_edges;
      };

      class Graph : public AbstractGraph {
      public:
        Graph(IM adjacency_matrix, IM fixed, bool digraph = FALSE);
        Graph(IV in_degrees, IV out_degrees, IM f, bool digraph = FALSE);
      private:
        std::vector<Vertex> vertices_;
        Edge** edges_;
        void updateAdjacencyMatrix() override;
        void sampleStep() override;
      };
    }
    namespace Undirected {



    }

  }

  namespace Switch {
    class Graph : public AbstractGraph {

    };
  }

  namespace Matching {
    class Graph : public AbstractGraph {

    };
  }

  namespace DG {
    class Graph : public AbstractGraph {

    };
  }
}

#endif


//
// class Graph {
// public:
//   Graph(Rcpp::IntegerMatrix adjacency_matrix, Rcpp::IntegerMatrix fixed,
//      bool digraph = FALSE);
//   Graph(Rcpp::IntegerVector in_degrees, Rcpp::IntegerVector out_degrees,
//      Rcpp::IntegerMatrix f, bool digraph = FALSE);
//   Rcpp::List sample(std::string method, int nsamples, int thin, int burnin);
//   void printData();
//   void printEdgeList();
//   void printStubList();
//   Rcpp::IntegerMatrix adjacency_matrix() { return adjacency_matrix_; };
//   Rcpp::IntegerMatrix fixed() { return fixed_; };
//
// private:
//   std::vector<int> one_nums_, zero_nums_, in_stubs_, out_stubs_;
//   bool directed_;
//   std::vector<UnWeighted::Edge> edge_list_;
//   std::vector<std::vector<int> > ones_, zeros_;
//   std::discrete_distribution<int> one_dist_;
//   std::vector<std::uniform_int_distribution<int> > one_sampler_, zero_sampler_;
//   std::vector<std::vector<std::vector<int> > > tracking_;
//   std::default_random_engine generator_;
//   int nrow_, ncol_, nstubs_;
//   signed int rejected_;
//   Rcpp::IntegerMatrix adjacency_matrix_;
//   Rcpp::IntegerMatrix fixed_;
//   void updateAdjacencyMatrix();
//   void switchStep();
//   void DGStep();
//   void SGStep();
//   void matchingStep();
//   void updateData();
// };
