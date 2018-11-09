  // Overlapping Community Structure
#ifndef OVERLAPPING_STRUCTURE_H
#define OVERLAPPING_STRUCTURE_H

#include <vector>
#include <utility>
#include <cmath>
#include <map>
#include "Graph.h"
#include "Clustering.h"
#include <iostream>
#include <cassert>

using namespace std;

#define FUZZY_OVERLAP 0
#define CRISP_OVERLAP 1
#define DISJOINT_OVERLAP 2

#define FUZZY_PER_NODE 0
#define FUZZY_PER_COMM 1

typedef vector<pair<int, double> > Cover;

class OverlappingStructure {

public:
  static Cover *fuzzy(Clustering &c) {
    
    Graph &g = *(c.graph);
    int n = g.num_vertices;
    Cover *C = new Cover[n];
    int *freq = new int[n];
    int *in_freq = new int[n];
    int freq_sz = 0;
    
    for (int u = 0; u < n; u++) {
      set<pair<int, double> > s;
      s.insert(make_pair(c.getMembership(u), 0));
      
      for (int i = 0; i < g.adj_sz[u]; i++) {
        int v = g.adj[u][i];
        int com = c.getMembership(v);
        s.insert(make_pair(com, 0));
      }
      C[u] = Cover(s.begin(), s.end());
      freq[u] = 0;
    }
    for (int u = 0; u < n; u++) {
      double max_connectivity = 0.0;
      freq[c.getMembership(u)]++;
      in_freq[freq_sz++] = c.getMembership(u);
      
      for (int i = 0; i < g.adj_sz[u]; i++) {
        int v = g.adj[u][i];
        int com = c.getMembership(v);
        double connectivity = (member(C[u], com).second += g.adj_sim[u][i]);
        max_connectivity = max_connectivity < connectivity ? connectivity : max_connectivity;
        freq[com]++;
        in_freq[freq_sz++] = com;
      }
      for (auto &p : C[u]) {
        p.second = p.second/max_connectivity*freq[p.first]/c.getClusterSize(p.first);
      }
      for (;freq_sz;) {
        freq[in_freq[--freq_sz]] = 0;
      }
    }
    delete [] freq;
    delete [] in_freq;
    return C;
  }
  
  static vector<int> *crisp(Clustering &c, Cover *fuzzy, double threshold = 0.05) {
    
    Graph &g = *(c.graph);
    int n = g.num_vertices;
    vector<int> *C = new vector<int>[n];
    
    for (int u = 0; u < n; u++) {
      for (auto &p : fuzzy[u]) {
        if (p.second >= threshold) {
          C[u].push_back(p.first);
        }
      }
      if (C[u].empty()) {
        C[u].push_back(u);
      }
    }
    return C;
  }
  
  static int *disjoint(Clustering &c, Cover *fuzzy) {
    
    Graph &g = *(c.graph);
    int n = g.num_vertices;
    int *C = new int[n];
    bool *visited = new bool[n];
    int *st = new int[n];
    int st_sz = 0;
    
    for (int u = 0; u < n; u++) {
      visited[u] = 0;
      double maxi = -1.0;
      
      for (auto &p : fuzzy[u]) {
        if (maxi < p.second) {
          maxi = p.second;
          C[u] = p.first;
        }
      }
    }
    for (int u = 0; u < n; u++) {
      if (!visited[u]) {
        int target = C[u];
        st_sz = 0;
        st[st_sz++] = u;
        
        while (st_sz > 0) {
          int v = st[--st_sz];
          C[v] = u;
          
          for (int i = 0; i < g.adj_sz[v]; i++) {
            int k = g.adj[v][i];
            
            if (C[k] == target && !visited[k]) {
              st[st_sz++] = k;
              visited[k] = true;
            }
          }
        }
      }
    }
    delete [] visited;
    delete [] st;
    return C;
  }
  
private:
  static inline pair<int, double> &member(Cover &c, int com) {
    
    int mid, lo = 0, hi = c.size()-1;
    
    while(lo < hi) {
      mid = (lo+hi)>>1;
      
      if (c[mid].first < com) {
        lo = mid+1;
      } else {
        hi = mid;
      }
    }
    return c[lo].first == com ? c[lo] : dummy;
  }
  
  static pair<int, double> dummy;
};

pair<int, double> OverlappingStructure::dummy = make_pair(-1, 0.0f);

#endif

