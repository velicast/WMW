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
  static pair<Clustering *, double **> cluster(Graph &g, int comm_def = WEAK_CLUSTER, int min_size = 3, int iss_iters = 2) {
    
    Clustering *clus = new Clustering(&g);
    double **extS;
    double *S = DynamicStructuralSimilarity::fixedPoint(g, iss_iters, &extS);
    
    if (comm_def == WEAK_CLUSTER) {
      weakCluster(*clus, S);
    }
    else if (comm_def == MOST_WEAK_CLUSTER) {
      mostWeakCluster(*clus, S);
    }
    minClusterSize(*clus, S, min_size);
    delete [] S;
    return make_pair(clus, extS);
  }
  
private:
  static void weakCluster(Clustering &c, double *S) {
    
    Graph &g = *(c.graph);
    int n = g.num_vertices;
    int m = g.num_edges;
    int *B = new int[n];
    double *maxS = new double[n];
    vector<int> *to_merge = new vector<int>[n];
    int *E = new int[n];
    bool merged;
    
    do {
      merged = false;

      for (int i = 0; i < n; i++) {
        B[i] = E[i] = 0;
        maxS[i] = -1;
        to_merge[i].clear();
      }
      for (int i = 0; i < m; i++) {
        int cu = c.getMembership(g.src[i]);
        int cv = c.getMembership(g.dst[i]);
        
        if (cu != cv) {
          B[cu] -= 2;
          B[cv] -= 2;
          E[cu]++;
          E[cv]++;
          updateCandidates(maxS, S[i], cu, cv, to_merge);
        }
        B[cu]++;
        B[cv]++;
      }
      for (int i = 0; i < n; i++) {
        if (B[i] < 0 && !isOverlap(E[i], to_merge[i])) {
          for (int com : to_merge[i]) {
            merged |= c.merge(i, com) != -1;
          }
        }
      }
    } while (merged);

    delete [] B;
    delete [] E;
    delete [] maxS;
    delete [] to_merge;
  }

  static void mostWeakCluster(Clustering &c, double *S) {
  
    Graph &g = *(c.graph);
    int n = g.num_vertices;
    int m = g.num_edges;
    unsigned long long hn = n;
    unordered_map<unsigned long long, int> H(m);
    int *B = new int[n];
    int *D = new int[n];
    int *E = new int[n];    
    double *maxS = new double[n];
    vector<int> *to_merge = new vector<int>[n];
    bool merged;
    
    do {
      merged = false;
      H.clear();
      
      for (int i = 0; i < n; i++) {
        B[i] = D[i] = E[i] = 0;
        maxS[i] = -1;
        to_merge[i].clear();        
      }
      for (int i = 0; i < m; i++) {
        int cu = c.getMembership(g.src[i]);
        int cv = c.getMembership(g.dst[i]);

        if (cu == cv) {
          B[cu] += 2;
        } else {
          int f = ++H[cu <= cv ? cu*hn+cv : cv*hn+cu];

          if (f > D[cu]) {
            D[cu] = f;
          }
          if (f > D[cv]) {
            D[cv] = f;
          }
          E[cu]++;
          E[cv]++;
          updateCandidates(maxS, S[i], cu, cv, to_merge);
        }
      }
      for (int i = 0; i < n; i++) {
        if (B[i] < D[i] && !isOverlap(E[i], to_merge[i])) {
          for (int com : to_merge[i]) {
            merged |= c.merge(i, com) != -1;
          }
        }
      }
    } while (merged);

    delete [] B;
    delete [] D;
    delete [] E;
    delete [] maxS;
  }

  static void minClusterSize(Clustering &c, double *S, int k) {

    Graph &g = *(c.graph);
    int n = g.num_vertices;
    int m = g.num_edges;
    int *size = new int[n];
    int *E = new int[n];
    double *maxS = new double[n];
    vector<int> *to_merge = new vector<int>[n];

    bool merged, done;

    do {
      merged = false;
      done = true;
      
      for (int i = 0; i < n; i++) {
        E[i] = 0;
        maxS[i] = 0.0;
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
          E[cu]++;
          E[cv]++;
          updateCandidates(maxS, S[i], cu, cv, to_merge);          
        }
      }
      for (int i = 0; i < n; i++) {
        if (size[i] < k && !isOverlap(E[i], to_merge[i])) {
          for (int com : to_merge[i]) {
            merged |= c.merge(i, com) != -1;
          }
        }
      }
    } while (merged);
    
    delete [] size;
    delete [] E;
    delete [] maxS;
    delete [] to_merge;
  }

  static void updateCandidates(double *maxS, double newS, int cu, int cv, vector<int> *to_merge) {
    
    if (flt(maxS[cu], newS)) {
      maxS[cu] = newS;
      to_merge[cu].clear();
      to_merge[cu].push_back(cv);
    }
    else if (feq(maxS[cu], newS)) {
      to_merge[cu].push_back(cv);
    }
    if (flt(maxS[cv], newS)) {
      maxS[cv] = newS;
      to_merge[cv].clear();
      to_merge[cv].push_back(cu);
    }
    else if (feq(maxS[cv], newS)) {
      to_merge[cv].push_back(cu);
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
