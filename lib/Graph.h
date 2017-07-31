#ifndef GRAPH_H
#define GRAPH_H

#include <algorithm>
#include <vector>
#include <unordered_set>
#include <cstdlib>
#include <cfloat>

typedef unsigned long long Key64;

using namespace std;

class Graph {

public:
  Graph(int num_vertices) {

    this->num_vertices = num_vertices;
    this->adj = new int*[num_vertices];
    this->adj_s = new float*[num_vertices];
    this->adj_sz = new int[num_vertices];
  }

  ~Graph() {

    for (int i = 0; i < num_vertices; i++) {
      delete [] adj[i];
      delete [] adj_s[i];
    }
    delete [] adj;
    delete [] adj_s;
    delete [] adj_sz;
    delete [] src;
    delete [] dst;
  }

  Key64 eHash(Key64 u, Key64 v) {
    
    if (u > v) {
      u ^= v;
      v ^= u;
      u ^= v;
    }
    return u*num_vertices+v;
  }

  void eUnhash(Key64 h, int &u, int &v) {
  
    u = h/num_vertices;
    v = h%num_vertices;
  }
  
  void addEdges(vector<int> &v_src, vector<int> &v_dst) {
    
    this->num_edges = v_src.size();
    vector<int> *adj_tmp = new vector<int>[num_vertices];
    unordered_set<Key64> eh(num_edges);

    for (int i = 0; i < num_edges; i++) {
      if (v_src[i] == v_dst[i]) {
        continue;
      }
      Key64 h = eHash(v_src[i], v_dst[i]);

      if (!eh.count(h)) {
        eh.insert(h);
        adj_tmp[v_src[i]].push_back(v_dst[i]);
        adj_tmp[v_dst[i]].push_back(v_src[i]);
      }
    }
    for (int i = 0; i < num_vertices; i++) {
      adj_sz[i] = adj_tmp[i].size();
      adj[i] = new int[adj_sz[i]];
      adj_s[i] = new float[adj_sz[i]];
      memset(adj_s[i], 0, adj_sz[i]*sizeof(float));
      copy(adj_tmp[i].begin(), adj_tmp[i].end(), adj[i]);
      sort(adj[i], adj[i]+adj_sz[i]);
    }
    delete [] adj_tmp;
    src = new int[eh.size()];
    dst = new int[eh.size()];
    num_edges = 0;
    
    for (Key64 h : eh) {
      eUnhash(h, src[num_edges], dst[num_edges]);
      num_edges++;
    }
    eh.clear();
  }
  
  int num_vertices;
  int num_edges;
  int *src;
  int *dst;
  int **adj;
  int *adj_sz;
  float **adj_s;
};

#endif
