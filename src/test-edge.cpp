#include <testthat.h>
#include <Rcpp.h>
#include <stdexcept>
#include "weighted_graph.h"
using namespace std;
using namespace Rcpp;

context("Edge"){

  test_that("Initialiser updates Vertex and Edge data") {
    Vertex h, t1, t2, tf;
    int w1 = 1, w2 = 1, wf = 1;
    Edge e1(&h, &t1, &w1, false);
    Edge ef(&h, &tf, &wf, true);
    Edge e2(&h, &t2, &w2, false);

    expect_true(h.in_edges.size() == 2);
    expect_true(h.in_edges[0] == &e1);
    expect_true(h.in_edges[1] == &e2);
    expect_true(e1.pos() == 0);
    expect_true(ef.pos() == -1);
    expect_true(e2.pos() == 1);
  }

  test_that("weight() from zero to pos updates Vertex and Edge data") {
    Vertex h, t;
    int w = 0;
    Edge e(&h, &t, &w, false);
    e.weight(1);

    expect_true(h.in_edges[0] == &e);
    expect_true(e.pos() == 0);
  }

  test_that("weight() from pos to zero updates Vertex and Edge data") {
    Vertex h, t1, t2;
    int w1 = 1, w2 = 1;
    Edge e1(&h, &t1, &w1, false);
    Edge e2(&h, &t2, &w2, false);
    e1.weight(0);

    expect_true(h.in_edges.size() == 1);
    expect_true(h.in_edges[0] == &e2);
    expect_true(e2.pos() == 0);
  }
}
