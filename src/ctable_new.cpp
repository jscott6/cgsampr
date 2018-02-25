
#include "ctable_new.h"
#include "AuxiliaryFunctions.h"

using namespace Rcpp;
using namespace std;

/* constructors */

ctable::ctable(Rcpp::IntegerMatrix x0, Rcpp::IntegerMatrix f):
  nrow(x0.nrow()),
  ncol(x0.ncol()),
  kmax(min(nrow,ncol)),
  lowbound(0),
  upbound(0),
  lowfactor(0.),
  upfactor(0.),
  table_total(0),
  x(x0),
  fixed(f),
  c(std::vector<unsigned int>(ncol)),
  constraints(vector<constraint>()),
  entries(vector<vector<entry> >(nrow, vector<entry>())),
  cycle(vector<entry*>(2*kmax)),
  end(cycle.begin()){

  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  generator = default_random_engine(seed);

  // initialise constraints...
  for(int i=0;i!=nrow;++i)
    constraints.push_back(constraint());
  for(int j=0;j!=ncol;++j)
    constraints.push_back(constraint());

  // initialise entries
  for(unsigned int i=0;i!=nrow;++i)
    for(unsigned int j=0;j!=ncol;++j)
      entries[i].push_back(entry(i,j,&constraints[i],&constraints[nrow+j],&x(i,j)));

  // add entries to row constraints if not equal to column marginals
  for(unsigned int i=0; i!=nrow; ++i)
    for(unsigned int j=0; j!=ncol; ++j)
      if(fixed(i,j)==0){
        add_row(constraints[i], &entries[i][j]);
        table_total+=x(i,j);
      }

  // add entries to col constraints if positive
  for(unsigned int j=0; j!=ncol; ++j)
    for(unsigned int i=0; i!=nrow;++i)
      if(fixed(i,j)==0 && x(i,j)>0)
        add_col(constraints[nrow+j], &entries[i][j]);

}


/* methods */

// adds entry to a row constraint
void ctable::add_row(constraint& cp, entry* ep){
  cp.add_entry(ep);
  ep->set_row_loc(cp.get_last());
}

// adds entry to a column constraint
void ctable::add_col(constraint& cp, entry* ep){
  cp.add_entry(ep);
  ep->set_col_loc(cp.get_last());
}

// adds entry pointer to cycle at specified position if not seen before
void ctable::update_cycle(vector<entry*>::iterator& it, entry*& ep){
  if(ep->get_visited()==false){
    *it=ep; ++it;
    ep->set_visited(true);
  }
}

// resets multiplier and sampled properties of cycle entries
void ctable::reset_cycle(){
  for(vector<entry*>::iterator it=cycle.begin(); it!=end;++it){
    (*it)->reset();
  }
}

void ctable::get_cycle(){

  uniform_int_distribution<unsigned int> k_dist(2,kmax);
  unsigned int k = k_dist(generator);

  vector<entry*>::iterator it = cycle.begin();
  uniform_int_distribution<unsigned int> col_dist(nrow,nrow+ncol-1);
  Rcout << "SAMPLED ENTRIES: " << endl << endl;
  // sample first entry
  entry* ep = constraints[col_dist(generator)].sample();
  ep->increment_multiplier();
  Rcout << "(" << ep->get_row() << ", " << ep->get_col() << ") " << endl;
  update_cycle(it, ep);

  for(int i=1; i!=k;++i){
    // sample from row constraint
    ep = ((ep)->get_row_constraint())->sample(ep);
    ep->decrement_multiplier();
    Rcout << "(" << ep->get_row() << ", " << ep->get_col() << ") " << endl;
    update_cycle(it, ep);
    // sample from column constraint
    ep = ((ep)->get_col_constraint())->sample(ep);
    ep->increment_multiplier();
    Rcout << "(" << ep->get_row() << ", " << ep->get_col() << ") " << endl;
    update_cycle(it, ep);
  }
  // sample final entry
  ep = &entries[ep->get_row()][cycle.front()->get_col()];
  ep->decrement_multiplier();
  Rcout << "(" << ep->get_row() << ", " << ep->get_col() << ") " << endl;
  update_cycle(it,ep);
  end = it;

  calc_bounds();
  reset_cycle();

}


void ctable::calc_bounds(){

  // need some way of initialising upbound and lowbound.
  // set initial values to greater than total in table (invalid to have higher)
  lowbound = table_total;
  upbound = table_total;
  Rcout << "CYCLE ENTRIES AND FACTORS: " << endl << endl;
  Rcout << " LowBound: " << lowbound << " UpBound: " << upbound << endl;
  for(vector<entry*>::iterator it=cycle.begin(); it!=end;++it){
    (*it)->update_bound(upbound, up);
    (*it)->update_bound(lowbound,low);
      Rcout << "(" << (*it)->get_row() << ", " << (*it)->get_col() << ")";
      Rcout << " Value: " << (*it)->get_value();
      Rcout << " Multiplier: " << (*it)->get_multiplier();
      Rcout << " LowBound: " << lowbound << " UpBound: " << upbound << endl;
  }
}

// calculates factors for end case
void ctable::calc_factor(){


}


/*

void ctable::get_cycle(){

  // set cycle length
  uniform_int_distribution<unsigned int> k_dist(2,kmax);
  end = cycle.begin() + 2*k_dist(generator);

  // find initial entry
  vector<entry*>::iterator it = cycle.begin();
  uniform_int_distribution<unsigned int> col_dist(nrow,nrow+ncol-1);
  *it = constraints[col_dist(generator)].sample();

  while(it!=(end-2)){
    *(it+1) = ((*it)->get_row_constraint())->sample(*it);
    ++it;
    *(it+1) = ((*it)->get_col_constraint())->sample(*it);
    ++it;
  }

  *(it+1) = &entries[(*it)->get_row()][cycle[0]->get_col()];

  // print out selected cycle!!

  calc_bounds();
  calc_factor();

}


*/

/*
void ctable::calc_bounds(){

  //begin by resetting visits to zero. Initialise first and last to 1
  for(vector<entry*>::iterator e= cycle.begin();e!=end;++e)
    (*e)->set_visits(0);

  vector<entry*>::iterator e = cycle.begin();
  constraint* first = (*e)->get_col_constraint();
  first_col.push(*e);
  // initialise up and low
  Rcout << "CYCLE DATA: " << endl<<endl;
  lowbound = (*e)->get_value();
  (*e)->increment_visits();
  Rcout << "(" << (*e)->get_row() << ", " << (*e)->get_col() << ") ";
  Rcout << "Low Bound: " << lowbound << " Visits: " << (*e)->get_visits() << endl;
  e++;
  upbound = (*e)->get_value();
  (*e)->increment_visits();
  Rcout << "(" << (*e)->get_row() << ", " << (*e)->get_col() << ") ";
  Rcout << "Up Bound: " << upbound << " Visits: " << (*e)->get_visits() << endl;
  e++;
  while(e!=end){
    (*e)->increment_visits();
    lowbound = min(lowbound, (*e)->get_value()/(*e)->get_visits());
    Rcout << "(" << (*e)->get_row() << ", " << (*e)->get_col() << ") ";
    Rcout << "Low Bound: " << lowbound << " Visits: " << (*e)->get_visits() << endl;
    e++;
    (*e)->increment_visits();
    upbound = min(upbound,(*e)->get_value()/(*e)->get_visits());
    Rcout << "(" << (*e)->get_row() << ", " << (*e)->get_col() << ") ";
    Rcout << "Up Bound: " << upbound << " Visits: " << (*e)->get_visits() << endl;
    e++;
    if((*(e-1))->get_col_constraint()==first){
        if((*(e-1))->get_visits()==1)
          first_col.push(*(e-1));
        if(e != end){
          if((*e)->get_visits()==0)
            first_col.push(*e);
      }
    }
  }
  Rcout << endl<<"Final Up Bound: " << upbound << ", Final Low Bound: " << lowbound << endl;
  Rcout << endl << endl;
}



// given a cycle, computed the factors for edge cases
void ctable::calc_factor(){

  Rcout << "ELEMENTS ON FIRST COLUMN: " << endl << endl;

  while(first_col.size()!=0){
    Rcout << "(" << first_col.front()->get_row() << ", " << first_col.front()->get_col() << ")" << endl;
    first_col.pop();
  }
  Rcout << endl;
}
*/

/*
float n_int = (cycle.front()->get_col_constraint())->size();


if((*(end-1))->get_value() == 0)
  n_int += 1;
float n_up = n_int, n_low = n_int;

if((*(end-1))->get_value() == upbound*((*(end-1))->get_visits()))
  n_up -= 1;
if(cycle.front()->get_value() == lowbound*(cycle.front()->get_visits()))
  n_low -=1;

Rcout << "n_int: " << n_int << endl;
Rcout << "n_up: " << n_up << endl;
Rcout << "n_low: " << n_low << endl;
*/

/*
void ctable::update_x(){


}

// performs one whole sampling step
void ctable::sample_step(){


}
*/




/*
void ctable::remove_row(constraint* cp, entry**epp){
  cp.remove_entry(epp);

}

void ctable::remove(constraint* cp, entry** epp){
  cp.remove_entry(epp);

}
*/
