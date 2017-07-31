#ifndef COSINE_SIMILARITY_H
#define COSINE_SIMILARITY_H

#include <cmath>
#include "Graph.h"

using namespace std;

class CosineSimilarity {

public:
  static void unweighted(Graph &g) {
    
    float t = 0;
    int x, y, u, v, ux, vy, iu, iv;

    for (int i = 0; i < g.num_edges; i++, t = 0) {
      u = g.src[i];
      v = g.dst[i];
    
      for (x = 0, y = 0, iu = -1, iv = -1; x < g.adj_sz[u] && y < g.adj_sz[v];) {
        ux = g.adj[u][x];
        vy = g.adj[v][y];

        if (ux == v) {
          iv = x;
        }
        if (vy == u) {
          iu = y;
        }
        if (ux == vy) {
          t++;
          x++;
          y++;
        }
        else if (ux < vy) {
          x++;
        }
        else {
          y++;
        }
      }
      for (; iv == -1 && x < g.adj_sz[u]; x++) {
        if (g.adj[u][x] == v) {
          iv = x;
        }
      }
      for (; iu == -1 && y < g.adj_sz[v]; y++) {
        if (g.adj[v][y] == u) {
          iu = y;
        }
      }
      if (t > 0) {
        t = t/sqrt(1.*(g.adj_sz[u]-1)*(g.adj_sz[v]-1));
        g.adj_s[u][iv] = g.adj_s[v][iu] = t;
      }
    }
  }
};

#endif
