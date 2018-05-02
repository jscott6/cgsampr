
#include "edge.h"
#include <algorithm>
using namespace std;

Edge::Edge(Vertex* const h,Vertex* const t, int* const w, const bool f) :
  head_(h),
  tail_(t),
  fixed_(f),
  weight_(w),
  pos_(-1),
  visits_(STAR) {
  if (*weight_ > 0 && !fixed_) add();
  if (!fixed_) tail_->out_edges.push_back(this);
  return;
}

void Edge::increment() {
  if (visits_ == STAR) visits_ = 1;
  else visits_++;
  return;
}

void Edge::decrement() {
  if (visits_ == STAR) visits_ = -1;
  else visits_--;
  return;
}

// adds edge to in 'in edges' of head Vertex
// assumes edge is NOT already referenced in Vertex data
// will only be called as part of weight()
void Edge::add() {
  pos_ = head_->in_edges.size();
  head_->in_edges.push_back(this);
}

// removes edge from 'in edges' of a Vertex
// assumes edge is referenced in Vertex at location pos
// will only be called as part of setWeight
void Edge::remove() {
    head_->in_edges.back()->pos_ = pos_;
    swap(head_->in_edges[pos_], head_->in_edges.back());
    head_->in_edges.pop_back();
}

void Edge::weight(int w) {
  if (*weight_ == 0 && w > 0) add();
  if (*weight_ > 0 && w == 0) remove();
  *weight_ = w;
}
