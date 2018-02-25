
/*

#include "ctable.h"
#include "AuxiliaryFunctions.h"

using namespace Rcpp;
using namespace std;





ctable::ctable(Rcpp::IntegerMatrix x0, Rcpp::IntegerMatrix f):
  nrow(x0.nrow()),
  ncol(x0.ncol()),
  kmax(min(nrow,ncol)),
  x(x0),
  fixed(f),
  constraints(vector<vector<unsigned int> >)),
  elements(vector<entry*> kmax),
  end(elements.begin());
  {
    // set generator
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    generator = default_random_engine(seed);

    unsigned int k=0;
    for(unsigned int i=0; i!=nrow; ++i){
      for(unsigned int j=0; j!=ncol; ++j){
        if(fixed(i,j)==0){
          elements.push_back(entry(k,i,j));
          ++k;
          if(x(i,j)!=c[j]){
            add_entry(constraints)[i]
          }
          if(x(i,j)>0){
            add_entry(constraints[nrow+j],elements[k]);
          }
        }

        }
      }



}


ctable::add_entry(k,l){
  constraints[k].push_back(elements[l]);
}



ctable::ctable(Rcpp::IntegerMatrix x0, Rcpp::IntegerMatrix f):
  nrow(x0.nrow()),
  ncol(x0.ncol()),
  kmax(min(nrow,ncol)),
  x(x0),
  fixed(f),
  rows(vector<vector<unsigned int> > (nrow, vector<unsigned int>(0))),
  cols(vector<vector<unsigned int> > (ncol, vector<unsigned int>(0))),
  rowNums(vector<unsigned int>(nrow,0)),
  colNums(vector<unsigned int>(ncol,0)),
  c(vector<unsigned int>(nrow,0)),
  oddEntries(vector<entry>(kmax)),
  end(oddEntries.begin()),
  tracking(vector<vector<vector<unsigned int> > >
  (ncol, vector<vector<unsigned int> >(nrow, vector<unsigned int>(2,0)))),
  colDist(uniform_int_distribution<unsigned int>(0,ncol-1)),
  lenDist(uniform_int_distribution<unsigned int>(2,kmax))
  {
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  generator = default_random_engine(seed);

  //need to compute column marginals.
  for(int j=0;j<ncol;++j)
    for(int i=0;i<nrow;++i)
      c[j] += x(i,j);

  for(int i=0; i<nrow; ++i){
    for(int j=0; j<ncol; ++j){
      // only consider non fixed elements
      if(fixed(i,j)==0){
        if(x(i,j)>0){
          // add to col structure if positive
          cols[j].push_back(i);
          colNums[j]++;
        }

        if(x(i,j)!=c[j]){
          // add to row structure if not equal to column marginal
          rows[i].push_back(j);
          rowNums[i]++;
        }
      }
    }
  }
}


void ctable::print_data(){

  Rcout << "Positive Row entries: " << endl;
  for(int i=0; i<ncol; ++i){
    Rcout << "Column " << i << ": ";
    printVec(cols[i]);
  }
  Rcout << "col entries: " << endl;
  for(int j=0; j<ncol; ++j){
    Rcout << "Row " << j << ": ";
    printVec(rows[j]);
  }
  Rcout << endl;
  Rcout << "ColNums: "<< endl;
  printVec(colNums);
  Rcout << "RowNums: " << endl;
  printVec(rowNums);
  return;
}

// samples *new* column on given row
// assumes more than one possible new column
unsigned int ctable::sample_col(const unsigned int& i, const unsigned int& j){
  uniform_int_distribution<unsigned int> dist(0, rowNums[i]-2);
  unsigned int idx = dist(generator);
  if(rows[i][idx]==j)
    idx = rowNums[i]-1;
  return rows[i][idx];
}

// samples *new* row on given column
// assumes more than one possible new row
unsigned int ctable::sample_row(const unsigned int& i,const unsigned int& j){
  uniform_int_distribution<unsigned int> dist(0, colNums[j]-2);
  unsigned int idx = dist(generator);
  if(cols[j][idx]==i)
    idx = colNums[j]-1;
  return cols[j][idx];
}


void ctable::get_cycle(){

  // generate cycle length
  end = oddEntries.begin() + lenDist(generator);

  // pick first entry
  vector<entry>::iterator it = oddEntries.begin();
  it->col = colDist(generator);
  uniform_int_distribution<unsigned int> dist(0, colNums[it->col]-1);
  it->row = cols[it->col][dist(generator)];

  while(it!=end){
    ++it;
    it->col = sample_col((it-1)->row, (it-1)->col);
    it->row = sample_row((it-1)->row, it->col);
  }

  //print cycle !!

  for(vector<entry>::iterator it=oddEntries.begin(); it!=(end-1);++it){
    // print odd element
    Rcout << "(" << it->row+1 << ", " << it->col+1 << ")" << endl;
    Rcout << "(" << it->row+1 << ", " << (it+1)->col+1 << ")" << endl;
  }

  // final two
  Rcout << "(" << (end-1)->row+1 << ", " << (end-1)->col+1 << ")" << endl;
  Rcout << "(" << (end-1)->row+1 << ", " << oddEntries.begin()->col+1 << ")" << endl;


}
// return a tuple of upper and lower bound
// shouldn't need to merge loops as c++ doesn't appear to have a great overhead
// in this respect. Makes sense then to maintain modularity of code
void ctable::calc_range(){

}


// we will need to calculate the factor along the upper and lower bound.
// this may prove more involved than we expect.


// given delta, adjust table along cylce elements.
// this method should be almost trivial

*/
