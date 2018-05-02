

#include <testthat.h>
#include <Rcpp.h>
#include "weightedGraph.h"
#include "AuxiliaryFunctions.h"

using namespace std;
using namespace Rcpp;

context("WeightedGraph") {

  test_that("Initialiser exits gracefully with prohibited input") {
    int s=5;
    // test wrong dimensions
    IntegerMatrix x0(s,s), f0(s,6);
    expect_error_as(weightedGraph(x0,f0), invalid_argument);
    // test x has negative entries
    IntegerMatrix x1(s,s), f1(s,s);
    x1(2,3) = -1;
    expect_error_as(weightedGraph(x1,f1), invalid_argument);
    // test f not binary
    IntegerMatrix x2(s,s), f2(s,s);
    f2(1,3) = 2;
    expect_error_as(weightedGraph(x2,f2), invalid_argument);
  }

  test_that("Initialiser initialises edge pos member correctly "){
    IntegerMatrix x(5,5), f(5,5);
    int nrow = x.nrow(), ncol = x.ncol();
    fill(x.begin(), x.end(), 10);
    weightedGraph wg(x,f);
    edge** edges = wg.get_edges();
    bool res = true;
    for(int i=0; i!=nrow; ++i){
      for(int j=0; j!=ncol;++j){
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
    edge** edges = wg.get_edges();
    expect_true(edges[2][3].get_pos() == -1);
  }

  test_that("Initialiser throws error when matrix determined by specification"){
    IntegerMatrix x(5,5) , f(5,5);
    fill(f.begin(), f.end(), 1);
    expect_error_as(weightedGraph(x,f), invalid_argument);
  }

  test_that("sampleKernel() cycle length"){
    vector<edge*> cycle;
    IntegerMatrix x(5,5), f(5,5);
    fill(x.begin(), x.end(), 10);
    // length should be four when no elements are fixed
    weightedGraph wg(x,f);
    printMatrix(x);
    printMatrix(f);
    wg.sampleKernel(cycle);
    //expect_true(cycle.size()==4);

    vector<edge*> cycle1;
    IntegerMatrix x1(3,3), f1(3,3);
    fill(x1.begin(),x1.end(),10);
    for(int i=0;i!=3;++i)
      f1(i,i)=1;
    // length must be 6 in this setup
    //weightedGraph wg1(x1,f1);
    //wg.sampleKernel(cycle1);
    expect_true(cycle1.size()==6);
  }

}
