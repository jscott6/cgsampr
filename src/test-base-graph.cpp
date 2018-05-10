
#include <testthat.h>
#include <Rcpp.h>
#include <stdexcept>
#include "interface.h"
#include "AuxiliaryFunctions.h"

using namespace std;
using namespace Rcpp;
using namespace Base;

context("Graph") {

  test_that("Initialiser from adjacency matrix exits gracefully with prohibited input") {
    int s = 5;
    // test wrong dimensions
    IntegerMatrix x0(s, s), f0(s, 6);
    expect_error_as(Graph(x0, f0, true), invalid_argument);
    // test x has negative entries
    IntegerMatrix x1(s, s), f1(s, s);
    x1(2,3) = -1;
    expect_error_as(Graph(x1, f1, true), invalid_argument);
    // test f not binary
    IntegerMatrix x2(s, s), f2(s, s);
    f2(1,3) = 2;
    expect_error_as(Graph(x2, f2, true), invalid_argument);

    IntegerMatrix x(5, 5) , f(5, 5);
    fill(f.begin(), f.end(), 1);
    expect_error_as(Graph(x, f, true), invalid_argument);
  }

  test_that("Initialiser from degrees exits gracefully with prohibited input") {
    int s = 5;



  }

}
