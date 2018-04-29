
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

#endif
