
#include "ctable_new.h"

using namespace Rcpp;
using namespace std;

constraint::constraint()
  : entries(vector<entry*>()){
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  generator = default_random_engine(seed);
};

void constraint::add_entry(entry* ep){
  entries.push_back(ep);
}

void constraint::remove_entry(unsigned int i){
  swap(entries[i], entries.back());
  entries.pop_back();
}

unsigned int constraint::size(){
  return entries.size();
}

unsigned int constraint::get_last(){
  return entries.size()-1;
}

entry* constraint::sample(entry* &ep){
  uniform_int_distribution<unsigned int> dist(0, entries.size()-2);
  unsigned int idx = dist(generator);
  if(entries[idx]==ep)
    return entries.back();
  return entries[idx];
}

entry* constraint::sample(){
  uniform_int_distribution<unsigned int> dist(0, entries.size()-1);
  unsigned int idx = dist(generator);
  return entries[idx];
}
