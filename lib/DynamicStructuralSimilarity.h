  // Dynamic Structural Similarity
#ifndef DYNAMIC_STRUCTURAL_SIMILARITY_H
#define DYNAMIC_STRUCTURAL_SIMILARITY_H

#include "Graph.h"
#include <iostream>
#include <climits>
#include <iomanip>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <vector>
using namespace std;

#define feq(a, b) (fabs(a-b) <= max(abs(a), abs(b))*DBL_EPSILON || fabs(a-b) <= DBL_EPSILON)
#define flt(a, b) (!feq(a, b) && a-b < 0.0)
#define fgt(a, b) (!feq(a, b) && a-b > 0.0)

template<typename Functor>
void parallel_range(int start, int end, Functor&& f, int num_threads = thread::hardware_concurrency()) {

  atomic<int> index(0);
  thread *ts = new thread[num_threads];

  for (int i = 0; i < num_threads; i++) {
    ts[i] = thread([&start, &end, &index, &f]() {
      int i = index.fetch_add(1);
      while(i < end) {
        f(i);
        i = index.fetch_add(1);
      }
    });
  }
  for (int i = 0; i < num_threads; i++) {  
    ts[i].join();
  }
  delete [] ts;
}


class DynamicStructuralSimilarity {

public:
  static void fixedPoint(Graph &g, int iters = 2) {
     
    int n = g.num_vertices;
    int m = g.num_edges;
    double **next_sim = new double*[n];
    
    for (int u = 0; u < n; u++) {
      next_sim[u] = new double[g.adj_sz[u]];
    }
    for (; iters--;) {
      parallel_range(0, m, [&g, &next_sim](int i) {
        iterate(g, i, next_sim);
      });
      /*for (int e = 0; e < m; e++) {
        iterate(g, e, next_sim);
      }*/
      double **tmp = g.adj_sim;
      g.adj_sim = next_sim;
      next_sim = tmp;
    }
    for (int u = 0; u < n; u++) {
      delete [] next_sim[u];
    }
    delete [] next_sim;
  }

private:
  static void iterate(Graph &g, int e, double **next_sim) {
    
    double s = 0, sim_u = 0, sim_v = 0;
    int i = 0, j = 0, u = g.src[e], v = g.dst[e], x, y, uj = -1, vi = -1;
    
    for (; i < g.adj_sz[u] && j < g.adj_sz[v];) {
      x = g.adj[u][i];
      y = g.adj[v][j];

      if (x == v) {
        vi = i;
      }
      if (y == u) {
        uj = j;
      }
      if (x == y) {
        s += g.adj_sim[u][i];
        s += g.adj_sim[v][j];
        sim_u += g.adj_sim[u][i];
        sim_v += g.adj_sim[v][j];
        i++;
        j++;
      }
      else if (x < y) {
        sim_u += g.adj_sim[u][i];
        i++;
      }
      else {
        sim_v += g.adj_sim[v][j];
        j++;
      }
    }
    for (; i < g.adj_sz[u]; i++) {
      sim_u += g.adj_sim[u][i];
      
      if (g.adj[u][i] == v) {
        vi = i;
      }
    }
    for (; j < g.adj_sz[v]; j++) {
      sim_v += g.adj_sim[v][j];
    
      if (g.adj[v][j] == u) {
        uj = j;
      }
    }
    double num = s+g.adj_sim[u][vi]+g.adj_sim[v][uj];
    double den = sqrt(sim_u*sim_v);
    g.sim[e] = next_sim[u][vi] = next_sim[v][uj] = num/den;
  }
};

#endif


