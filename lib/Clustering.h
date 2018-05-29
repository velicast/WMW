#ifndef CLUSTERING_H
#define CLUSTERING_H

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
  
  Graph *graph;
  DisjointSet *clusters;
};

#endif
