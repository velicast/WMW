  // Weak Communities
#ifndef WEAK_COMMUNITY_DETECTION_H
#define WEAK_COMMUNITY_DETECTION_H

#include "Clustering.h"
#include "DynamicStructuralSimilarity.h"
#include <cstring>
#include <utility>
#include <cmath>
#include <limits>

using namespace std;

class WeakCommunityDetection {

public:
  static Clustering *cluster(Graph &g, int min_size = 3, int dss_iters = 2) {
    
    Clustering *clus = new Clustering(&g);
    DynamicStructuralSimilarity::fixedPoint(g, dss_iters);
    weakCluster(*clus, min_size);
    return clus;
  }
  
private:
  static void weakCluster(Clustering &c, int k) {
    
    Graph &g = *(c.graph);
    int n = g.num_vertices;
    int m = g.num_edges;
    double *B = new double[n];
    double *max_sim = new double[n];
    vector<int> *to_merge = new vector<int>[n];
    int *size = new int[n];
    bool merged;
    
    do {
      merged = false;

      for (int i = 0; i < n; i++) {
        B[i] = 0;
        max_sim[i] = -1;
        to_merge[i].clear();
        size[i] = c.getClusterSize(i);
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
        if (flt(B[i], 0.0) || size[i] < k) {
          for (int com : to_merge[i]) {
            merged |= c.merge(i, com) != -1;
          }
        }
      }
    } while (merged);

    delete [] B;
    delete [] max_sim;
    delete [] to_merge;
    delete [] size;    
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
