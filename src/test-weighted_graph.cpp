#include <testthat.h>
#include <Rcpp.h>
#include <stdexcept>
#include "weighted_graph.h"
#include "AuxiliaryFunctions.h"
#include <utility>
#include <string>
#include <fstream>
using namespace std;
using namespace Rcpp;
using namespace Weighted::Directed::SG;
using namespace Weighted;


// void getVerticesCycleData(const vector<Edge *> &vec,const DeltaRange& dr);
// bool populateTestVertexData(vector<pair<int, int>> coords, vector<int> weights, vector<CycleData> expected);
// void printCycleData(const CycleData& cycle_data);
// bool testBoundaryWeights(vector<pair<int, int>> coords, vector<int> weights, Factor expected);

context("Graph") {

  // test_that("Initialiser initialises Edge pos member correctly ") {
  //   IntegerMatrix x(5, 5), f(5, 5);
  //   int nrow = x.nrow(), ncol = x.ncol();
  //   fill(x.begin(), x.end(), 10);
  //   Graph wg(x, f);
  //   Edge** edges = wg.edges();
  //   bool res = true;
  //   for (int i = 0; i != nrow; ++i){
  //     for(int j = 0; j != ncol; ++j){
  //       Edge* e = &edges[i][j];
  //       Edge* p = e->head_->in_edges[e->pos()];
  //       if(p != e)
  //         res = false;
  //     }
  //   }
  //   expect_true(res);
  // }

  // test_that("Initialiser ignores fixed_ edges"){
  //   IntegerMatrix x(5, 5), f(5, 5);
  //   fill(x.begin(), x.end(), 10);
  //   f(2, 3) = 1;
  //   Graph wg(x, f);
  //   Edge** edges = wg.edges();
  //   expect_true(edges[2][3].pos() == -1);
  // }

  // test_that("sampleKernel() cycle valid"){
  //   int nsamples = 1000;
  //   IntegerMatrix x(5, 5), f(5, 5), x1(3, 3), f1(3, 3);
  //   fill(x.begin(), x.end(), 10);
  //   fill(x1.begin(),x1.end(),10);
  //   for (int i = 0; i != 3; ++i) f1(i,i)=1;
  //   Graph wg(x, f);
  //   Graph wg1(x1, f1);
  //   // length should be four when no elements are fixed_
  //   bool len = true, len_fixed = true, pattern=true, newedge=true;
  //   for (int i = 0; i != nsamples; ++i) {
  //     vector<Edge*> cycle, cycle1;
  //     wg.sampleKernel(cycle);
  //     wg.updateWeights(cycle, 0);
  //     wg1.sampleKernel(cycle1);
  //     wg1.updateWeights(cycle1, 0);
  //     if (cycle.size() != 4) len = false;
  //     if (cycle1.size() != 6) len_fixed = false;
  //     for (int k = 0; k != cycle.size(); ++k) {
  //       if (k % 2) {
  //         if (k != cycle.size()-1 && cycle[k]->head_ != cycle[k+1]->head_)
  //           pattern = false;
  //       }
  //       else {
  //         if (cycle[k]->tail_ != cycle[k+1]->tail_)
  //           pattern = false;
  //         }
  //       if (k != 0 && cycle[k] == cycle[k-1]) newedge = false;
  //     }
  //   }
  //   expect_true(len);
  //   expect_true(len_fixed);
  //   expect_true(pattern);
  //   expect_true(newedge);
  // }

  // test_that("Vertex cycle data populates as expected")
  // {
  //   // test case 1
  //   expect_true(populateTestVertexData({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 1}, {3, 1}, {3, 0}},
  //                          {2, 1, 1, 2, 2, 1, 1, 2},
  //                          {{1, 0, 0, 0, 0},
  //                           {2, 2, 2, 2, 2},
  //                           {1, 0, 0, 0, 0},
  //                           {0, 0, 0, 0, 0}}));

  //   // test case 2
  //   expect_true(populateTestVertexData({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 1}, {3, 1}, {3, 0}},
  //                          {2, 1, 2, 2, 2, 2, 1, 2},
  //                          {{1, 0, 0, 0, 0},
  //                           {2, 1, 1, 1, 1},
  //                           {1, 0, 0, 0, 0},
  //                           {0, 0, 0, 0, 0}}));

  //   // test case 3
  //   expect_true(populateTestVertexData({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 1}, {3, 1}, {3, 0}},
  //                          {2, 2, 2, 1, 1, 2, 2, 2},
  //                          {{1, 0, 0, 0, 0},
  //                           {2, 0, 0, 0, 0},
  //                           {1, 1, 1, 1, 1},
  //                           {0, 0, 0, 0, 0}}));

  //   // test case 4
  //   expect_true(populateTestVertexData({{0, 0}, {0, 1}, {1, 1}, {1, 0}},
  //                          {1, 2, 2, 1},
  //                          {{1, 1, 1, 1, 1},
  //                           {1, 0, 0, 0, 0}}));

  //   // test case 5
  //   expect_true(populateTestVertexData({{0, 0}, {0, 1}, {1, 1}, {1, 0}},
  //                          {2, 1, 1, 2},
  //                          {{1, 0, 0, 0, 0},
  //                           {1, 1, 1, 1, 1}}));

  //   // test case 6
  //   expect_true(populateTestVertexData({{0, 0}, {0, 1}, {1, 1}, {1, 0}},
  //                          {1, 1, 1, 1},
  //                          {{1, 1, 1, 1, 1},
  //                           {1, 1, 1, 1, 1}}));

  //   //test case 7
  //   expect_true(populateTestVertexData({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 1}, {3, 1}, {3, 2}, {4, 2}, {4, 1}, {5, 1}, {5, 0}},
  //                          {2, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 2},
  //                          {{1, 0, 0, 0, 0},
  //                           {3, 2, 2, 2, 2},
  //                           {2, 0, 0, 0, 0},
  //                           {0, 0, 0, 0, 0},
  //                           {0, 0, 0, 0, 0},
  //                           {0, 0, 0, 0, 0}}));

  //   //test case 8
  //   expect_true(populateTestVertexData({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 1}, {0, 1}, {0, 0}},
  //                          {2, 2, 1, 2, 2, 1, 2, 2},
  //                          {{1, 0, 0, 0, 0},
  //                           {2, 1, 1, 1, 1},
  //                           {1, 0, 0, 0, 0}}));

  //   //test case 9
  //   expect_true(populateTestVertexData({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {0, 2}, {0, 1}, {2, 1}, {2, 0}},
  //                          {2, 2, 2, 2, 2, 2, 1, 2},
  //                          {{1, 0, 0, 0, 0},
  //                           {2, 1, 2, 1, 1},
  //                           {1, 0, 0, 0, 0}}));
  // }

  // test_that("Generates expected boundary weights for test scenarios")
  // {
  //   expect_true(testBoundaryWeights({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 1}, {3, 1}, {3, 0}},
  //                          {2, 1, 1, 2, 2, 1, 1, 2},
  //                          {9./8., 9./8.}));

  //   expect_true(testBoundaryWeights({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 1}, {3, 1}, {3, 0}},
  //                          {2, 1, 2, 2, 2, 2, 1, 2},
  //                          {3./4., 3./4.}));

  //   expect_true(testBoundaryWeights({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 1}, {3, 1}, {3, 0}},
  //                          {2, 2, 2, 1, 1, 2, 2, 2},
  //                          {1./2.,1./2.}));

  //   expect_true(testBoundaryWeights({{0, 0}, {0, 1}, {1, 1}, {1, 0}},
  //                          {1, 2, 2, 1},
  //                          {1,1}));

  //   expect_true(testBoundaryWeights({{0, 0}, {0, 1}, {1, 1}, {1, 0}},
  //                          {2, 1, 1, 2},
  //                          {1./2.,1./2.}));

  //   expect_true(testBoundaryWeights({{0, 0}, {0, 1}, {1, 1}, {1, 0}},
  //                          {1, 1, 1, 1},
  //                          {1,1}));

  //   expect_true(testBoundaryWeights({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 1}, {3, 1}, {3, 2}, {4, 2}, {4, 1}, {5, 1}, {5, 0}},
  //                          {2, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 2},
  //                          {125./96., 125./96.}));

  //   expect_true(testBoundaryWeights({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 1}, {0, 1}, {0, 0}},
  //                          {2, 2, 1, 2, 2, 1, 2, 2},
  //                          {1,1}));

  //   expect_true(testBoundaryWeights({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {0, 2}, {0, 1}, {2, 1}, {2, 0}},
  //                          {2, 2, 2, 2, 2, 2, 1, 2},
  //                          {1,1./2.}));
  //   }

}

// bool populateTestVertexData(vector<pair<int, int>> coords, vector<int> weights, vector<CycleData> expected)
// {
//   int n = 0;
//   for (const auto &coord : coords)
//     n = max(n, max(coord.first, coord.second) + 1);
//   IntegerMatrix adj(n, n), fixed(n, n);
//   for (int i = 0; i != coords.size(); i++)
//     adj(coords[i].first, coords[i].second) = weights[i];
//   Graph wg(adj, fixed);
//   Edge **edges = wg.edges();
//   vector<Edge *> cycle;
//   int i = -1;
//   for (const auto &coord : coords)
//   {
//     vector<Edge*>::iterator iter = find(cycle.begin(), cycle.end(), &edges[coord.first][coord.second]);
//     if (iter == cycle.end())
//       cycle.push_back(&edges[coord.first][coord.second]);
//     if ((++i) % 2)
//       edges[coord.first][coord.second].incrementOdd();
//     else
//       edges[coord.first][coord.second].incrementEven();
//   }

//   DeltaRange dr = wg.getDeltaRange(cycle);
//   getVerticesCycleData(cycle, dr);
//   bool good = true;
//   for (const auto& e : cycle)
//   {
//     int idx = e->head_->index - n;
//     if (
//         e->head_->cycle_data.visits != expected[idx].visits ||
//         e->head_->cycle_data.up_zeros != expected[idx].up_zeros ||
//         e->head_->cycle_data.up_zero_visits != expected[idx].up_zero_visits ||
//         e->head_->cycle_data.low_zeros != expected[idx].low_zeros ||
//         e->head_->cycle_data.low_zero_visits != expected[idx].low_zero_visits)
//         good = false;
//   }
//   return good;
// }


// bool testBoundaryWeights(vector<pair<int, int>> coords, vector<int> weights, Factor expected)
// {
//   int n = 0;
//   for (const auto &coord : coords)
//     n = max(n, max(coord.first, coord.second) + 1);
//   IntegerMatrix adj(n, n), fixed(n, n);
//   for (int i = 0; i != coords.size(); i++)
//     adj(coords[i].first, coords[i].second) = weights[i];
//   Graph wg(adj, fixed);
//   Edge **edges = wg.edges();
//   vector<Edge *> cycle;
//   int i = -1;
//   for (const auto &coord : coords)
//   {
//     vector<Edge*>::iterator iter = find(cycle.begin(), cycle.end(), &edges[coord.first][coord.second]);
//     if (iter == cycle.end())
//       cycle.push_back(&edges[coord.first][coord.second]);
//     if ((++i) % 2)
//       edges[coord.first][coord.second].incrementOdd();
//     else
//       edges[coord.first][coord.second].incrementEven();
//   }

//   DeltaRange dr = wg.getDeltaRange(cycle);
//   Factor factor = wg.getBoundaryWeights(cycle, dr);

//   if(abs(factor.low - expected.low)<1e-6 && abs(factor.up - expected.up) < 1e-6)
//     return true;
//   else
//     return false;
// }
