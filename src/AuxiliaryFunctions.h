
#ifndef GUARD_AuxiliaryFunctions
#define GUARD_AuxiliaryFunctions

#include <Rcpp.h>
#include <vector>
#include <iomanip>
#include "weightedGraphs.h"

void printVec(std::vector<int> );
void printVec(std::vector<unsigned int> vec);
void printMatrix(Rcpp::IntegerMatrix );
void printVertexData(vertex &v);
template class<T>
T sampleFromVector(vector<T> &vec, default_random_engine gen);
template class<T>
T sampleNewFromVector(vector<T> &vec, T x, default_random_engine gen);

#endif
