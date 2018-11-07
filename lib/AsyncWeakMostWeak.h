  // Asynchronous Weak Most Weak Communities
#ifndef ASYNC_WEAK_MOST_WEAK_H
#define ASYNC_WEAK_MOST_WEAK_H

#include "Clustering.h"
#include "ExtDisjointSet.h"
#include "WeakMostWeak.h"
#include "DynamicStructuralSimilarity.h"
#include <utility>
#include <cstring>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

class AsyncWeakMostWeak {

public:
  static pair<Clustering *, double **> cluster(Graph &g, int comm_def = WEAK_CLUSTER, int min_size = 3, int iss_iters = 2) {
    
    int n = g.num_vertices;
    double **extS;
    ExtDisjointSet *ds;
    
    delete [] DynamicStructuralSimilarity::fixedPoint(g, iss_iters, &extS);
    
    if (comm_def == WEAK_CLUSTER) {
      ds = weakCluster(g, extS);
    }
    else if (comm_def == MOST_WEAK_CLUSTER) {
      ds = mostWeakCluster(g, extS);
    }
    else {
      ds = new ExtDisjointSet(n);
    }
    minClusterSize(g, ds, extS, min_size);
    Clustering *clus = new Clustering(&g);
    delete clus->clusters;
    clus->clusters = ds->getBasicDS();
    delete ds;
    return make_pair(clus, extS);
  }
  
private:
  static ExtDisjointSet *weakCluster(Graph &g, double **S) {
    
    int n = g.num_vertices;
    int m = g.num_edges;
    int *I = new int[n];
    int *E = new int[n];
    int *C = new int[n];
    bool *done = new bool[n];
    bool merged;
    vector<int> to_merge;    
    ExtDisjointSet *ds = new ExtDisjointSet(n);
    
    for (int i = 0; i < n; i++) {
      I[i] = 0;
      E[i] = g.adj_sz[i];
      done[i] = false;
    }
    do {
      merged = false;
      int nc = 0;
      
      for (DSSetIterator it = ds->sets(); it.hasNext();) {
        int cu = it.next();

        if (I[cu] < E[cu]) {
          C[nc++] = cu;
        }
      }
      for (int i = 0; i < nc; i++) {
        int cu = C[i], rc = -1, adjacents = 0;
        double max_w = -1;
        
        if (done[cu]) {
          continue;
        }
        for (DSMemberIterator it = ds->members(cu); it.hasNext();) {
          for (int j = 0, u = it.next(); j < g.adj_sz[u]; j++) {
            int cv = ds->find(g.adj[u][j]);
            
            if (cu != cv) {
              updateCandidates(max_w, S[u][j], cv, to_merge);            
              adjacents++;
            }
          }
        }
        if (!isOverlap(adjacents, to_merge)) {
          for (int c : to_merge) {
            int k = 0;
            c = ds->find(c);
            
            for (DSMemberIterator it = ds->members(cu); it.hasNext();) {
              for (int j = 0, u = it.next(); j < g.adj_sz[u]; j++) {
                int cv = ds->find(g.adj[u][j]);
                k += cv == c;
              }
            }
            int l = ds->merge(c, cu);
          
            if (l == -1) {
              continue;
            }
            int child = (l == c) ? cu : c;
            done[child] = true;
            I[l] += I[child]+2*k;
            E[l] += E[child]-2*k;
            merged |= I[l] < E[l];
          }
        }
        to_merge.clear();
      }
      for (int i = 0; i < n; i++) {
        done[i] = false;
      }
    } while (merged);

    delete [] done;
    delete [] I;
    delete [] E;
    delete [] C;
    return ds;
  }
  
  static ExtDisjointSet* mostWeakCluster(Graph &g, double **S) {
    
    int n = g.num_vertices;
    int m = g.num_edges;
    int *F = new int[n];
    int *C = new int[n];
    bool merged;
    ExtDisjointSet *ds = new ExtDisjointSet(n);
    vector<int> to_merge;
    vector<int> T;
    
    memset(F, 0, n*sizeof(int));
    
    do {
      merged = false;
      int nc = 0;
      
      for (DSSetIterator it = ds->sets(); it.hasNext();) {
        C[nc++] = it.next();
      }   
      for (int i = 0; i < nc; i++) {
        int cu = C[i], rc = -1, I = 0, E = 0, adjacents = 0;
        double max_w = -1;
            
        for (DSMemberIterator it2 = ds->members(cu); it2.hasNext();) {
          for (int j = 0, u = it2.next(); j < g.adj_sz[u]; j++) {
            int cv = ds->find(g.adj[u][j]);
            
            if (cu != cv) {
              if (++F[cv] > E) {
                E = F[cv];
              }
              if (F[cv] == 1) {
                T.push_back(cv);
              }
              updateCandidates(max_w, S[u][j], cv, to_merge);
              adjacents++;
            } else {
              I++;
            }
          }
        }
        if (I < E && !isOverlap(adjacents, to_merge)) {
          for (int c : to_merge) {
            merged |= ds->merge(cu, c) != -1;
          }
        }
        for (int t : T) {
          F[t] = 0;
        }
        to_merge.clear();
        T.clear();
      }
    } while (merged);

    delete [] F;
    delete [] C;
    return ds;
  }

  static void minClusterSize(Graph &g, ExtDisjointSet *ds, double **S, int k) {
    
    int n = g.num_vertices;
    int m = g.num_edges;
    int nc = 0;
    int *C = new int[ds->num_sets];
    vector<int> to_merge;    
    bool merged;

    do {
      merged = false;
      int nc = 0;
      
      for (DSSetIterator it = ds->sets(); it.hasNext();) {
        int cu = it.next();

        if (ds->size(cu) < k) {
          C[nc++] = cu;
        }
      }
      for (int i = 0; i < nc; i++) {
        int cu = C[i], rc = -1, adjacents = 0;
        double max_w = -1;

        for (DSMemberIterator it2 = ds->members(cu); it2.hasNext();) {
          for (int j = 0, u = it2.next(); j < g.adj_sz[u]; j++) {
            int cv = ds->find(g.adj[u][j]);
            
            if (cu != cv) {
              updateCandidates(max_w, S[u][j], cv, to_merge);
              adjacents++;
            }
          }
        }
        if (!isOverlap(adjacents, to_merge)) {
          for (int c : to_merge) {
            merged = ds->merge(cu, c) != -1;
          }
        }
        to_merge.clear();
      }
    } while (merged);
  
    delete [] C;
  }

  static void updateCandidates(double &maxS, double newS, int cv, vector<int> &to_merge) {
    
    if (flt(maxS, newS)) {
      maxS = newS;
      to_merge.clear();
      to_merge.push_back(cv);
    }
    else if (feq(maxS, newS)) {
      to_merge.push_back(cv);
    }
  }

  static bool isOverlap(int external_connections, vector<int> &to_merge) {
    
    bool no = external_connections-to_merge.size();

    if (!no && to_merge.size()) {
      int mini_c = *min_element(to_merge.begin(), to_merge.end());
      int maxi_c = *max_element(to_merge.begin(), to_merge.end());
      no = feq(mini_c, maxi_c);
    }
    return !no;
  }
};

#endif


