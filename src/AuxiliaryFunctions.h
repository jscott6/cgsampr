
#ifndef GUARD_AuxiliaryFunctions
#define GUARD_AuxiliaryFunctions

#include <Rcpp.h>
#include <vector>
#include <iomanip>
#include "weighted_graph.h"

void printVec(std::vector<int> vec);
void printVec(std::vector<unsigned int> vec);
void printMatrix(Rcpp::IntegerMatrix mat);
void printVertexData(Vertex &v);
/*
template<class T>
T sampleFromVector(std::vector<T> &vec, std::default_random_engine gen);
template<class T>
T sampleNewFromVector(std::vector<T> &vec, T x, std::default_random_engine gen);
*/
#endif
