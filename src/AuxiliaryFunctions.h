
#ifndef GUARD_AuxiliaryFunctions
#define GUARD_AuxiliaryFunctions

#include <Rcpp.h>
#include <vector>
#include <iomanip>
#include <random>
#include "edge.h"
#include "graph.h"
#include "construct_graph.h"



void printMatrix(const Rcpp::IntegerMatrix mat);
void printVertexData(const Weighted::Vertex &v);
void printEdgeData(const Weighted::Edge &e);
Rcpp::IntegerMatrix constructGraph(Rcpp::IntegerVector, Rcpp::IntegerVector, Rcpp::IntegerMatrix);
void checkInputs(Rcpp::IntegerVector, Rcpp::IntegerVector, Rcpp::IntegerMatrix);
// prints a vector to output stream (assumes T printable)
template <class T>
void printVec(const std::vector<T> vec)
{
  for (typename std::vector<T>::const_iterator it = vec.begin();
       it != vec.end(); ++it)
  {
    Rcpp::Rcout << std::setprecision(2) << std::setw(2);
    Rcpp::Rcout << *it << " ";
  }
  Rcpp::Rcout << std::endl;
}

// generic function to sample from a vector
// given some vector, return a random (uniform) element from it
// Assumes vector is non-empty
template <class T>
T sampleFromVector(const std::vector<T> &vec, std::default_random_engine &gen)
{
  std::uniform_int_distribution<int> dist(0, vec.size() - 1);
  return vec[dist(gen)];
}

template <class T>
T * sampleReferenceFromVector(std::vector<T> &vec, std::default_random_engine &gen)
{
  std::uniform_int_distribution<int> dist(0, vec.size() - 1);
  return &(vec[dist(gen)]);
}


// generic function to sample NEW element from a vector
// given some vector, return a new random (uniform) element from it
// Assumes vector is of size 2 or more
// Assumes the element provided is in the vector
template <class T>
T sampleNewFromVector(const std::vector<T> &vec, T x, std::default_random_engine &gen)
{
  std::uniform_int_distribution<int> dist(0, vec.size() - 2);
  T res = vec[dist(gen)];
  if (res == x)
    res = vec.back();
  return res;
}

#endif
