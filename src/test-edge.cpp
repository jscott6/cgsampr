
#include <testthat.h>
#include <Rcpp.h>
#include "weightedGraph.h"

using namespace std;
using namespace Rcpp;

context("edge"){

  test_that("Initialiser updates vertex and edge data"){
    vertex h, t1, t2, tf;
    int w1 = 1, w2=1, wf=1;
    edge e1(&h,&t1,&w1,false);
    edge ef(&h,&tf,&wf, true);
    edge e2(&h,&t2,&w2,false);

    expect_true(h.p_in_edges.size()==2);
    expect_true(h.p_in_edges[0]==&e1);
    expect_true(h.p_in_edges[1]==&e2);
    expect_true(e1.get_pos()==0);
    expect_true(ef.get_pos()==-1);
    expect_true(e2.get_pos()==1);
  }


  test_that("set_weight() from zero to pos updates vertex and edge data"){
    vertex h, t;
    int w = 0;
    edge e(&h,&t,&w,false);

    e.set_weight(1);

    expect_true(h.p_in_edges[0]==&e);
    expect_true(e.get_pos()==0);
  }

  test_that("set_weight() from pos to zero updates vertex and edge data"){
    vertex h, t1, t2;
    int w1 = 1, w2=1;
    edge e1(&h,&t1,&w1,false);
    edge e2(&h,&t2,&w2,false);

    e1.set_weight(0);

    expect_true(h.p_in_edges.size()==1);
    expect_true(h.p_in_edges[0]==&e2);
    expect_true(e2.get_pos()==0);

  }

}
