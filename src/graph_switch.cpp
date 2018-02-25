

#include "graph_switch.h"
#include "StronglyConnectedComponents.h"
#include "FeasibleMatrix.h"
#include "AuxiliaryFunctions.h"

using namespace Rcpp;

IntegerMatrix graph_switch::get_fixed(){return fixed;};
Rcpp::IntegerMatrix graph::get_x(){return x;}


void graph_switch::init(IntegerMatrix x0, IntegerMatrix f){

  x = clone(x0);
  int nrow = x.nrow(); int ncol = x.ncol();
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator = std::default_random_engine(seed);

  scc_graph G(x0,f);
  fixed = G.fixed_values(f);
  fixed = f;

  for(int i=0; i<nrow; i++){
    for(int j=0; j<ncol; j++){
      arc e;
      e.head = i; e.tail = j; 
      arc_list.push_back(e);
    }
  }

}

graph_switch::graph_switch(IntegerMatrix x0, IntegerMatrix f){
  init(x0, f);
}

void graph_switch::sample_step(){
  return;
}

void graph_switch::print_arc_list(){
  for(arc::iterator it = edge_list.begin(); it!=edge_list.end()l it++)
    Rcout << "(" << it->head << ", " << it->tail << ")" << std::endl;
}
