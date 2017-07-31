#ifndef HAMUHI_H
#define HAMUHI_H

#include "Clustering.h"
#include <climits>
#include <iostream>
#include <utility>
#include <vector>
#include <iostream>

using namespace std;

#define WEAK_CLUSTER 0
#define WEAKEST_CLUSTER 1

class HAMUHI {

public:
  static void weakCluster(Clustering &c) {
    
    Graph &g = *(c.graph);
    int n = g.num_vertices;
    int m = g.num_edges;
    int *I = new int[n];
    int *E = new int[n];
    int *C = new int[n];
    bool merged;
    
    for (int i = 0; i < n; i++) {
      I[i] = 0;
      E[i] = g.adj_sz[i];
    }
    do {
      merged = false;
      int nc = 0;
      
      for (DSSetIterator it = c.getClusters(); it.hasNext();) {
        int cu = it.next();

        if (I[cu] < E[cu]) {
          C[nc++] = cu;
        }
      }
      for (int i = 0; i < nc; i++) {
        int cu = C[i], rc = -1;
        float max_w = -1;

        for (DSMemberIterator it = c.getMembers(cu); it.hasNext();) {
          for (int j = 0, u = it.next(); j < g.adj_sz[u]; j++) {
            int cv = c.getMembership(g.adj[u][j]);
            
            if (cu != cv && max_w < g.adj_s[u][j]) {
              max_w = g.adj_s[u][j];
              rc = cv;
            }
          }
        }
        if (rc == -1) {
          continue;
        }
        int k = 0;
        
        for (DSMemberIterator it = c.getMembers(cu); it.hasNext();) {
          for (int j = 0, u = it.next(); j < g.adj_sz[u]; j++) {
            int cv = c.getMembership(g.adj[u][j]);
            k += cv == rc;
          }
        }
        int l = c.merge(rc, cu);
        cu = (l == rc) ? cu : rc;
        I[l] += I[cu]+2*k;
        E[l] += E[cu]-2*k;
        merged |= I[l] < E[l];
      }
    } while (merged);

    delete [] I;
    delete [] E;
    delete [] C;
  }

  static void weakestCluster(Clustering &c) {
    
    Graph &g = *(c.graph);
    int n = g.num_vertices;
    int m = g.num_edges;
    int list_t_size = 0;
    int *F = new int[n];
    int *T = new int[n];
    int *C = new int[n];
    bool merged;

    memset(F, 0, n*sizeof(int));
    
    do {
      merged = false;
      int nc = 0;
      
      for (DSSetIterator it = c.getClusters(); it.hasNext();) {
        C[nc++] = it.next();
      }
      for (int i = 0; i < nc; i++) {
        int cu = C[i], rc = -1, I = 0, E = 0;
        float max_w = -1;
        
        for (DSMemberIterator it2 = c.getMembers(cu); it2.hasNext();) {
          for (int j = 0, u = it2.next(); j < g.adj_sz[u]; j++) {
            int cv = c.getMembership(g.adj[u][j]);
            
            if (cu != cv) {
              if (max_w < g.adj_s[u][j]) {
                max_w = g.adj_s[u][j];
                rc = cv;
              }
              if (++F[cv] > E) {
                E = F[cv];
              }
              if (F[cv] == 1) {
                T[list_t_size++] = cv;
              }
            } else {
              I++;
            }
          }
        }
        if (I <= E && rc >= 0) {
          c.merge(cu, rc);
          merged = true;
        }
        for (; list_t_size; list_t_size--) {
          F[T[list_t_size-1]] = 0;
        }
      }
    } while (merged);

    delete [] F;
    delete [] T;
    delete [] C;
  }

  static void minClusterSize(Clustering &c, int k) {
    
    Graph &g = *(c.graph);
    int n = g.num_vertices;
    int m = g.num_edges;
    int nc = 0;
    int *C = new int[c.getNumClusters()];
    bool merged;

    do {
      merged = false;
      int nc = 0;
      
      for (DSSetIterator it = c.getClusters(); it.hasNext();) {
        int cu = it.next();

        if (c.getClusterSize(cu) < k) {
          C[nc++] = cu;
        }
      }
      for (int i = 0; i < nc; i++) {
        int cu = C[i], rc = -1;
        float max_w = -1;
        
        for (DSMemberIterator it2 = c.getMembers(cu); it2.hasNext();) {
          for (int j = 0, u = it2.next(); j < g.adj_sz[u]; j++) {
            int cv = c.getMembership(g.adj[u][j]);
            
            if (cu != cv && max_w < g.adj_s[u][j]) {
              max_w = g.adj_s[u][j];
              rc = cv;
            }
          }
        }
        if (rc >= 0) {
          merged |= c.getClusterSize(c.merge(cu, rc)) < k;
        }
      }
    } while (merged);
  
    delete [] C;
  }
};

#endif


