#include <testthat.h>
#include <Rcpp.h>
#include <stdexcept>
#include "weighted_graph.h"
#include "AuxiliaryFunctions.h"
using namespace std;
using namespace Rcpp;
using namespace Weighted::Directed;
using namespace Weighted;

context("Graph") {

  test_that("Initialiser initialises Edge pos member correctly ") {
    IntegerMatrix x(5, 5), f(5, 5);
    int nrow = x.nrow(), ncol = x.ncol();
    fill(x.begin(), x.end(), 10);
    Graph wg(x, f);
    Edge** edges = wg.edges();
    bool res = true;
    for (int i = 0; i != nrow; ++i){
      for(int j = 0; j != ncol; ++j){
        Edge* e = &edges[i][j];
        Edge* p = e->head_->in_edges[e->pos()];
        if(p != e)
          res = false;
      }
    }
    expect_true(res);
  }

  test_that("Initialiser ignores fixed_ edges"){
    IntegerMatrix x(5, 5), f(5, 5);
    fill(x.begin(), x.end(), 10);
    f(2, 3) = 1;
    Graph wg(x, f);
    Edge** edges = wg.edges();
    expect_true(edges[2][3].pos() == -1);
  }

  test_that("sampleKernel() cycle valid"){
    int nsamples = 1000;
    IntegerMatrix x(5, 5), f(5, 5), x1(3, 3), f1(3, 3);
    fill(x.begin(), x.end(), 10);
    fill(x1.begin(),x1.end(),10);
    for (int i = 0; i != 3; ++i) f1(i,i)=1;
    Graph wg(x, f);
    Graph wg1(x1, f1);
    // length should be four when no elements are fixed_
    bool len = true, len_fixed = true, pattern=true, newedge=true;
    for (int i = 0; i != nsamples; ++i) {
      vector<Edge*> cycle, cycle1;
      wg.sampleKernel(cycle);
      wg.updateWeights(cycle, 0);
      wg1.sampleKernel(cycle1);
      wg1.updateWeights(cycle1, 0);
      if (cycle.size() != 4) len = false;
      if (cycle1.size() != 6) len_fixed = false;
      for (int k = 0; k != cycle.size(); ++k) {
        if (k % 2) {
          if (k != cycle.size()-1 && cycle[k]->head_ != cycle[k+1]->head_)
            pattern = false;
        }
        else {
          if (cycle[k]->tail_ != cycle[k+1]->tail_)
            pattern = false;
          }
        if (k != 0 && cycle[k] == cycle[k-1]) newedge = false;
      }
    }
    expect_true(len);
    expect_true(len_fixed);
    expect_true(pattern);
    expect_true(newedge);
  }
}
