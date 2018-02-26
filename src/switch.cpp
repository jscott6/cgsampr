
// employs switch method to sample a single step
void graph::switch_step(){

  int idx1, idx2;
  std::uniform_int_distribution<int> dist(0, arc_list.size());

  idx1 = dist(generator);
  idx2 = dist(generator);

  // edges must be distinct
  while(idx1 == idx2)
    idx2 = dist(generator);

  int head1 = arc_list[idx1].head;
  int head2 = arc_list[idx2].head;
  int tail1 = arc_list[idx1].tail;
  int tail2 = arc_list[idx2].tail;

  // delineated vertices must be distinct
  if(head1==head2 || head1==tail2 || head2==tail1 || tail1==tail2){
    return;
  }
  // no edges can already exist between heads and tails of different edges
  if(x(head1,tail2) == 1 || x(head2,tail1) == 1)
    return;

  // potential edges cannot be fixed...
  if(fixed(head1,tail2)==1 || fixed(head2,tail1)==1)
    return;


  Rcout << "(" << head1+1 << ", " << tail1+1 << ")" << std::endl;
  Rcout << "(" << head2+1 << ", " << tail2+1 << ")" << std::endl;

  // update edges
  arc_list[idx1].tail = tail2;
  arc_list[idx2].tail = tail1;

  // update x
  x(head1,tail1) = 0;
  x(head1,tail2) = 1;
  x(head2,tail1) = 1;
  x(head2,tail2) = 0;

  return;
}
