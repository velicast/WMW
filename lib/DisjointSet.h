#ifndef DISJOINT_SET_H
#define DISJOINT_SET_H

class DisjointSet {

public:
  DisjointSet(int n) {

    this->num_sets = n;
    set = new int[n];
    rank = new int[n];
    sz = new int[n];
    
    for (n--; n >= 0; n--) {
      set[n] = n;
      rank[n] = 0;
      sz[n] = 1;
    }
  }

  ~DisjointSet() {

    delete [] set;
    delete [] rank;
    delete [] sz;
  }

  int merge(int x, int y) {

    x = find(x);
    y = find(y);
    
    if (x == y) {
      return -1;
    }
    if (rank[x] <= rank[y]) {
      rank[y] += rank[x] == rank[y];
      x ^= y; y ^= x; x ^= y;
    }
    --num_sets;
    set[y] = x;
    sz[x] += sz[y];
    sz[y] = 0;
    return x;
  }

  int find(int x) {
    int i = x, j;

    while (i != set[i]) {
      i = set[i];
    }
    while (x != i) {
      j = set[x];
      set[x] = i;
      x = j;
    }
    return x;
  }

  int size(int x) {
    return sz[find(x)];
  }
  
  int num_sets;
  int *set;
  int *rank;
  int *sz;
};

#endif
