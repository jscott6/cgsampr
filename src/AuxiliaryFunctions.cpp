
#include "AuxiliaryFunctions.h"

using namespace Rcpp;

// prints an integer vector to output stream
void printVec(std::vector<int> vec){
  for(std::vector<int>::const_iterator it=vec.begin();
      it!=vec.end(); ++it){
        Rcout << std::setprecision(2) << std::setw(2);
        Rcout << *it << " ";
      }
  Rcout << std::endl;
}

// prints an integer vector to output stream
void printVec(std::vector<unsigned int> vec){
  for(std::vector<unsigned int>::const_iterator it=vec.begin();
      it!=vec.end(); ++it){
        Rcout << std::setprecision(2) << std::setw(2);
        Rcout << *it << " ";
      }
  Rcout << std::endl;
}

// prints a 2-d array to output stream
void printMatrix(IntegerMatrix vec){
  int n = vec.nrow();
  Rcout << std::endl;
  for(int i=0; i<n; i++){
    int m = vec.ncol();
    for(int j=0; j<m; j++){
      Rcout << std::setprecision(2) << std::setw(4);
      Rcout << vec(i,j) << " ";
    }
    Rcout << std::endl;
  }
  Rcout << std::endl;
}
