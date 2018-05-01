

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

}
