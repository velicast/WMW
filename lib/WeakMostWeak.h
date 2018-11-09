  // Weak Most Weak Communities
#ifndef WEAK_MOST_WEAK_H
#define WEAK_MOST_WEAK_H

#include "Clustering.h"
#include "DynamicStructuralSimilarity.h"
#include <cstring>
#include <unordered_map>
#include <utility>
#include <cmath>
#include <limits>

using namespace std;

#define NO_DEFINITION -1
#define WEAK_CLUSTER 0
#define MOST_WEAK_CLUSTER 1

class WeakMostWeak {

public:
  static Clustering *cluster(Graph &g, int comm_def = WEAK_CLUSTER, int min_size = 3, int dss_iters = 2) {
    
    Clustering *clus = new Clustering(&g);
    DynamicStructuralSimilarity::fixedPoint(g, dss_iters);
    
    if (comm_def == WEAK_CLUSTER) {
      weakCluster(*clus);
    }
    else if (comm_def == MOST_WEAK_CLUSTER) {
      mostWeakCluster(*clus);
    }
    minClusterSize(*clus, min_size);
    return clus;
  }
  
private:
  static void weakCluster(Clustering &c) {
    
    Graph &g = *(c.graph);
    int n = g.num_vertices;
    int m = g.num_edges;
    double *B = new double[n];
    double *max_sim = new double[n];
    vector<int> *to_merge = new vector<int>[n];
    bool merged;
    
    do {
      merged = false;

      for (int i = 0; i < n; i++) {
        B[i] = 0;
        max_sim[i] = -1;
        to_merge[i].clear();
      }
      for (int i = 0; i < m; i++) {
        int cu = c.getMembership(g.src[i]);
        int cv = c.getMembership(g.dst[i]);
        
        if (cu == cv) {
          B[cu] += 2*g.sim[i];
        } else {
          B[cu] -= g.sim[i];
          B[cv] -= g.sim[i];
          updateCandidates(max_sim, g.sim[i], cu, cv, to_merge);
        }
      }
      for (int i = 0; i < n; i++) {
        if (flt(B[i], 0.0)) {
          for (int com : to_merge[i]) {
            merged |= c.merge(i, com) != -1;
          }
        }
      }
    } while (merged);

    delete [] B;
    delete [] max_sim;
    delete [] to_merge;
  }

  static void mostWeakCluster(Clustering &c) {
  
    Graph &g = *(c.graph);
    int n = g.num_vertices;
    int m = g.num_edges;
    unsigned long long hn = n;
    unordered_map<unsigned long long, double> H(m);
    double *B = new double[n];
    double *D = new double[n];
    double *max_sim = new double[n];
    vector<int> *to_merge = new vector<int>[n];
    bool merged;
    
    do {
      merged = false;
      H.clear();
      
      for (int i = 0; i < n; i++) {
        B[i] = D[i] = 0;
        max_sim[i] = -1;
        to_merge[i].clear();
      }
      for (int i = 0; i < m; i++) {
        int cu = c.getMembership(g.src[i]);
        int cv = c.getMembership(g.dst[i]);

        if (cu == cv) {
          B[cu] += 2*g.sim[i];
        } else {
          double f = (H[cu <= cv ? cu*hn+cv : cv*hn+cu] += g.sim[i]);

          if (f > D[cu]) {
            D[cu] = f;
          }
          if (f > D[cv]) {
            D[cv] = f;
          }
          updateCandidates(max_sim, g.sim[i], cu, cv, to_merge);
        }
      }
      for (int i = 0; i < n; i++) {
        if (flt(B[i], D[i])) {
          for (int com : to_merge[i]) {
            merged |= c.merge(i, com) != -1;
          }
        }
      }
    } while (merged);

    delete [] B;
    delete [] D;
    delete [] max_sim;
  }

  static void minClusterSize(Clustering &c, int k) {

    Graph &g = *(c.graph);
    int n = g.num_vertices;
    int m = g.num_edges;
    int *size = new int[n];
    double *max_sim = new double[n];
    vector<int> *to_merge = new vector<int>[n];

    bool merged, done;

    do {
      merged = false;
      done = true;
      
      for (int i = 0; i < n; i++) {
        max_sim[i] = 0.0;
        size[i] = c.getClusterSize(i);
        done &= size[i] >= k;
        to_merge[i].clear();
      }
      if (done) {
        return;
      }
      for (int i = 0; i < m; i++) {
        int cu = c.getMembership(g.src[i]);
        int cv = c.getMembership(g.dst[i]);

        if (cu != cv) {
          updateCandidates(max_sim, g.sim[i], cu, cv, to_merge);          
        }
      }
      for (int i = 0; i < n; i++) {
        if (size[i] < k) {
          for (int com : to_merge[i]) {
            merged |= c.merge(i, com) != -1;
          }
        }
      }
    } while (merged);
    
    delete [] size;
    delete [] max_sim;
    delete [] to_merge;
  }

  static void updateCandidates(double *max_sim, double new_sim, int cu, int cv, vector<int> *to_merge) {
    
    if (flt(max_sim[cu], new_sim)) {
      max_sim[cu] = new_sim;
      to_merge[cu].clear();
      to_merge[cu].push_back(cv);
    }
    else if (feq(max_sim[cu], new_sim)) {
      to_merge[cu].push_back(cv);
    }
    if (flt(max_sim[cv], new_sim)) {
      max_sim[cv] = new_sim;
      to_merge[cv].clear();
      to_merge[cv].push_back(cu);
    }
    else if (feq(max_sim[cv], new_sim)) {
      to_merge[cv].push_back(cu);
    }
  }
};

#endif
