#ifndef MODULARITY_H
#define MODULARITY_H

#include "Graph.h"

class Modularity {

public:
  static double compute(Graph &g, int *membership) {
  
    int n = g.num_vertices;
    int m = g.num_edges;
    int *E = new int[n];
    int *A = new int[n];
    bool *C = new bool[n];
    double q = 0;

    for (int i = 0; i < n; i++) {
      E[i] = A[i] = C[i] = 0;
    }
    for (int i = 0, u, v, cu, cv; i < m; i++) {
      u = g.src[i];
      v = g.dst[i];
      cu = membership[u];
      cv = membership[v];

      if (cu == cv) {
        E[cu]++;
      }
      if (!C[u]) {
        A[cu] += g.adj_sz[u];
        C[u] = 1;
      }
      if (!C[v]) {
        A[cv] += g.adj_sz[v];
        C[v] = 1;
      }
    }
    for (int i = 0; i < n; i++) {
      q += 1.*E[i]/m-(1.*A[i]*A[i])/(4.*m*m);
    }
    delete [] E;
    delete [] A;
    delete [] C;
    return q;
  }
};

#endif
