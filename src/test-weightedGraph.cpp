

#include <testthat.h>
#include <Rcpp.h>
#include "weightedGraph.h"

using namespace std;
using namespace Rcpp;

context("WeightedGraph") {

  test_that("Initialiser exits gracefully with prohibited input") {
    // test wrong dimensions
    IntegerMatrix x0(5,5), f0(5,6);
    expect_error_as(weightedGraph(x0,f0), invalid_argument);
    // test x has negative entries
    IntegerMatrix x1(5,5), f1(5,5);
    x1(2,3) = -1;
    expect_error_as(weightedGraph(x1,f1), invalid_argument);
    // test f not binary
    IntegerMatrix x2(5,5), f2(5,5);
    f2(1,3) = 2;
    expect_error_as(weightedGraph(x2,f2), invalid_argument);
  }

  test_that("Initialiser initialises edge pos member correctly "){
    IntegerMatrix x(5,5), f(5,5);
    fill(x.begin(), x.end(), 10);
    weightedGraph wg(x,f);
    vector<edge>* edges = wg.get_edges();
    bool res = true;
    for(int i=0; i!=edges->size(); ++i){
      for(int j=0; j!=edges[0].size();++j){
        edge * e = &edges[i][j];
        edge * p = e->head()->p_in_edges[e->get_pos()];
        if(p!=e)
          res = false;
      }
    }
    expect_true(res);
  }

  test_that("Initialiser ignores fixed edges"){
    IntegerMatrix x(5,5), f(5,5);
    fill(x.begin(), x.end(), 10);
    f(2,3) = 1;
    weightedGraph wg(x, f);
    vector<edge>* edges = wg.get_edges();
    expect_true(edges[2][3].get_pos() == -1);
  }

  test_that("Initialiser throws error when matrix determined by specification"){
    IntegerMatrix x(5,5) , f(5,5);
    fill(f.begin(), f.end(), 1);
    expect_error_as(weightedGraph(x,f), invalid_argument);
  }



}
