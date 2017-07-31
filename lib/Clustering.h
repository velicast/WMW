#ifndef CLUSTERING_H
#define CLUSTERING_H

#include <vector>
#include <cstring>
#include <cstdlib>
#include "DisjointSet.h"
#include "Graph.h"

using namespace std;

class Clustering {

public:
  Clustering(Graph *g) {
    
    graph = g;
    clusters = new DisjointSet(g->num_vertices);
  }

  ~Clustering() {
    delete clusters;
  }

  int merge(int u, int v) {
    return clusters->merge(u, v);
  }
  
  int getClusterSize(int c) {
    return clusters->size(c);
  }
  
  int getMembership(int v) {
    return clusters->find(v);
  }

  int getNumClusters() {
    return clusters->num_sets;
  }

  DSSetIterator getClusters() {
    return clusters->sets();
  }

  DSMemberIterator getMembers(int c) {
    return clusters->members(c);
  }
  
  Graph *graph;
  DisjointSet *clusters;
};

#endif
