
#include "AuxiliaryFunctions.h"

using namespace Rcpp;
using namespace std;

// prints an integer vector to output stream
void printVec(::vector<int> vec){
  for(::vector<int>::const_iterator it=vec.begin();
      it!=vec.end(); ++it){
        Rcout << ::setprecision(2) << ::setw(2);
        Rcout << *it << " ";
      }
  Rcout << ::endl;
}

// prints an integer vector to output stream
void printVec(::vector<unsigned int> vec){
  for(::vector<unsigned int>::const_iterator it=vec.begin();
      it!=vec.end(); ++it){
        Rcout << ::setprecision(2) << ::setw(2);
        Rcout << *it << " ";
      }
  Rcout << ::endl;
}

// prints a 2-d array to output stream
void printMatrix(IntegerMatrix vec){
  int n = vec.nrow();
  Rcout << ::endl;
  for(int i=0; i<n; i++){
    int m = vec.ncol();
    for(int j=0; j<m; j++){
      Rcout << ::setprecision(2) << ::setw(4);
      Rcout << vec(i,j) << " ";
    }
    Rcout << ::endl;
  }
  Rcout << ::endl;
}



void printVertexData(vertex &v){

  Rcout <<  setw(8) << "In Edges" << endl;
  Rcout << "------------" << endl;
  Rcout <<  setw(8) << "Tail: ";
  for(int i=0; i!=v.p_in_edges.size();++i){
    Rcout << setw(2) << v.p_in_edges[i]->tail()->index << " ";
  }
  Rcout << endl;
  Rcout <<  setw(8) << "Head: ";
  for(int i=0; i!=v.p_in_edges.size();++i){
    Rcout << setw(2) << v.p_in_edges[i]->head()->index << " ";
  }
  Rcout << endl;
  Rcout <<  setw(8) << "Weight: ";
  for(int i=0; i!=v.p_in_edges.size();++i){
    Rcout << setw(2) << v.p_in_edges[i]->weight() << " ";
  }
  Rcout << endl;
  Rcout << endl;
  Rcout << setw(8) << "Possible Out Edges" << endl;
    Rcout << "------------" << endl;
  Rcout <<  setw(8) << "Tail: ";
  for(int i=0; i!=v.p_poss_out_edges.size();++i){
    Rcout << setw(2) << v.p_poss_out_edges[i]->tail()->index << " ";
  }
  Rcout << endl;
  Rcout <<  setw(8) << "Head: ";
  for(int i=0; i!=v.p_poss_out_edges.size();++i){
      Rcout << setw(2) << v.p_poss_out_edges[i]->head()->index << " ";
  }
  Rcout << endl;
  Rcout <<  setw(8) << "Weight: ";
  for(int i=0; i!=v.p_poss_out_edges.size();++i){
    Rcout << setw(2) << v.p_poss_out_edges[i]->weight() << " ";
  }
  Rcout << endl;
  return;
}


/*
generic function to sample from a vector
given some vector, return a random (uniform) element from it
Assumes vector is non-empty
*/
template class<T>
T sampleFromVector(vector<T> &vec, default_random_engine gen){
  uniform_int_distribution<int> dist(0, vec.size()-1);
  return vec[dist(gen)];
}

/*
generic function to sample NEW element from a vector
given some vector, return a new random (uniform) element from it
Assumes vector is of size 2 or more
*/
template class<T>
T sampleNewFromVector(vector<T> &vec, T x, default_random_engine gen){
  uniform_int_distribution<int> dist(0, vec.size()-2);
  T res = vec[dist(gen)];
  if(res==x)
    res = vec.back();
  return res;
}
