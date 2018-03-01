
/*
------------------------------------------------------------
Implementation of SG algorithm.
------------------------------------------------------------
*/

#include "graph.h"

using namespace Rcpp;

void graph::SG_step(){

  if(directed){
    // sample column randomly
    int j1 = one_dist(generator);
    int i1_idx = oneSampler[j1](generator);
    int j = j1, i = ones[j1][i1_idx];
    int i_ind, j_ind;

    while(true){
      // sample j_k from zeros i_{k-1}
      j_ind = zeroSampler[i](generator);
      std::swap(j, zeros[i][j_ind]);

      if(j == j1) break;

      // sample i_k from ones j_{k}
      i_ind = oneSampler[j](generator);
      std::swap(i, ones[j][i_ind]);
    }
    // need to change ones i1 from j1 to jk
    ones[j1][i1_idx] = i;
  }
  else{

    // initialise
    int i0 = one_dist(generator);
    int i0_idx = oneSampler[i0](generator);
    int idx, ip = i0, i = ones[i0][i0_idx];
    //Rcout << "i0: " << ip << " i: " << i << std::endl;
    int* t0, *t;
    if(ip<i){
      t0 = &tracking[ip][i][0];
      t = &tracking[ip][i][1];
    }else{
      t0 = &tracking[i][ip][1];
      t = &tracking[i][ip][0];
    }

    while(true){

      idx = zeroSampler[i](generator);
      ip = i;
      i = zeros[i][idx];
      //Rcout << "ip: " << ip << " i: " << i << std::endl;
      //print_data();
      // swap values
      std::swap(ones[ip][*t],zeros[ip][idx]);
      // also swap tracking matrix values
      if(ip<i){
        std::swap(*t, tracking[ip][i][0]);
        t = &tracking[ip][i][1];
      }
      else{
        std::swap(*t, tracking[i][ip][1]);
        t = &tracking[i][ip][0];
      }

      if(i == i0) break;

      idx = oneSampler[i](generator);
      ip = i;
      i = ones[i][idx];
      //Rcout << "ip: " << ip << " i: " << i << std::endl;
      //print_data();
      // swap values
      std::swap(zeros[ip][*t],ones[ip][idx]);
      // also swap tracking matrix values
      if(ip<i){
        std::swap(*t, tracking[ip][i][0]);
        t = &tracking[ip][i][1];
      }
      else{
        std::swap(*t, tracking[i][ip][1]);
        t = &tracking[i][ip][0];
      }
    }

    if(ip != ones[i0][i0_idx]){
      std::swap(ones[i0][i0_idx], zeros[i0][*t]);
      std::swap(*t0,*t);
    }

    //print_data();

  }
}

/*
List graph::sample_step_show_workings(unsigned int max_size){

  // sample column randomly
  int j1 = one_dist(generator);
  int i1_idx = oneSampler[j1](generator);
  int j = j1, i = ones[j1][i1_idx];
  int i1 = i;
  int i_ind, j_ind;
  List results(max_size);

  update_x();
  results(0) = clone(x);
  int l = 0;
  while(true){
    l = l+1;
    // sample j_k from zeros i_{k-1}
    j_ind = zeroSampler[i](generator);
    std::swap(j, zeros[i][j_ind]);
    update_x();
    x(i1,j1) = 0;
    results(l) = clone(x);

    if(j == j1) break;
    l = l+1;

    // sample i_k from ones j_{k}
    i_ind = oneSampler[j](generator);
    std::swap(i, ones[j][i_ind]);
    update_x();
    x(i1,j1) = 0;
    results(l) = clone(x);
  }
  // need to change ones i1 from j1 to jk
  ones[j1][i1_idx] = i;
  update_x();
  x(i1,j1) = 0;
  l = l+1;
  results(l) = clone(x);
  return results;
}
*/
