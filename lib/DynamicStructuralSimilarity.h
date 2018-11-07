  // Dynamic Structural Similarity
#ifndef DYNAMIC_STRUCTURAL_SIMILARITY_H
#define DYNAMIC_STRUCTURAL_SIMILARITY_H

#include "Graph.h"
#include <iostream>
#include <climits>
#include <iomanip>
#include <cstdlib>

using namespace std;

#define feq(a, b) (fabs(a-b) <= max(abs(a), abs(b))*DBL_EPSILON || fabs(a-b) <= DBL_EPSILON)
#define flt(a, b) (!feq(a, b) && a-b < 0.0)
#define fgt(a, b) (!feq(a, b) && a-b > 0.0)

class DynamicStructuralSimilarity {

public:
  static double *fixedPoint(Graph &g, int iters = 2, double ***resS = 0) {
     
    int n = g.num_vertices;
    int m = g.num_edges;
    double **currS = new double*[n];
    double **nextS = new double*[n];
    double *S = new double[m];
    
    for (int u = 0; u < n; u++) {
      currS[u] = new double[g.adj_sz[u]];
      nextS[u] = new double[g.adj_sz[u]];

      for (int i = 0; i < g.adj_sz[u]; i++) {
        currS[u][i] = 1.0;
      }
    }
    for (; iters--;) {
      for (int e = 0; e < m; e++) {
        S[e] = iterate(g, e, currS, nextS);
      }
      double **tmp = currS;
      currS = nextS;
      nextS = tmp;
    }
    if (resS == 0) {
      for (int u = 0; u < n; u++) {
        delete [] currS[u];
      }
      delete [] currS;
    } else {
      *resS = currS;
    }
    for (int u = 0; u < n; u++) {
      delete [] nextS[u];
    }
    delete [] nextS;
    return S;
  }

private:
  static double iterate(Graph &g, int i, double **currS, double **nextS) {
    
    double s = 0, sim_u = 0, sim_v = 0;
    int x = 0, y = 0, u = g.src[i], v = g.dst[i], ux, vy, iu = -1, iv = -1;
    
    for (; x < g.adj_sz[u] && y < g.adj_sz[v];) {
      ux = g.adj[u][x];
      vy = g.adj[v][y];

      if (ux == v) {
        iv = x;
      }
      if (vy == u) {
        iu = y;
      }
      if (ux == vy) {
        s += currS[u][x];
        s += currS[v][y];
        sim_u += currS[u][x];
        sim_v += currS[v][y];
        x++;
        y++;
      }
      else if (ux < vy) {
        sim_u += currS[u][x];
        x++;
      }
      else {
        sim_v += currS[v][y];
        y++;
      }
    }
    for (; x < g.adj_sz[u]; x++) {
      sim_u += currS[u][x];
      
      if (g.adj[u][x] == v) {
        iv = x;
      }
    }
    for (; y < g.adj_sz[v]; y++) {
      sim_v += currS[v][y];
    
      if (g.adj[v][y] == u) {
        iu = y;
      }
    }
    double num = s+currS[u][iv]+currS[v][iu];
    double den = sqrt(sim_u*sim_v);
    return nextS[u][iv] = nextS[v][iu] = num/den;
  }
};

#endif


