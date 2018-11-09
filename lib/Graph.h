#ifndef GRAPH_H
#define GRAPH_H

#include <algorithm>
#include <vector>
#include <set>
#include <cstdlib>
#include <cstring>
#include <cfloat>

typedef unsigned long long Key64;

using namespace std;

class Graph {

public:
  Graph(int num_vertices) {

    this->num_vertices = num_vertices;
    this->adj = new int*[num_vertices];
    this->adj_sz = new int[num_vertices];
    this->adj_sim = new double*[num_vertices];
  }

  ~Graph() {

    for (int i = 0; i < num_vertices; i++) {
      delete [] adj[i];
      delete [] adj_sim[i];
    }
    delete [] adj;
    delete [] adj_sz;
    delete [] adj_sim;
    delete [] src;
    delete [] dst;
    delete [] sim;
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
  
  void addEdges(vector<int> &v_src, vector<int> &v_dst, vector<double> &e_sim) {
    
    this->num_edges = v_src.size();
    vector<pair<int, double> > *adj_tmp = new vector<pair<int, double> >[num_vertices];
    set<pair<Key64, double> > eh;

    for (int i = 0; i < num_edges; i++) {
      if (v_src[i] == v_dst[i]) {
        continue;
      }
      Key64 h = eHash(v_src[i], v_dst[i]);
      auto p = make_pair(h, e_sim[i]);

      if (!eh.count(p)) {
        eh.insert(p);
        adj_tmp[v_src[i]].push_back(make_pair(v_dst[i], e_sim[i]));
        adj_tmp[v_dst[i]].push_back(make_pair(v_src[i], e_sim[i]));
      }
    }
    for (int i = 0; i < num_vertices; i++) {
      adj_sz[i] = adj_tmp[i].size();
      adj[i] = new int[adj_sz[i]];
      adj_sim[i] = new double[adj_sz[i]];
      sort(adj_tmp[i].begin(), adj_tmp[i].end());

      for (int j = 0; j < adj_sz[i]; j++) {
        adj[i][j] = adj_tmp[i][j].first;
        adj_sim[i][j] = adj_tmp[i][j].second;
      }
    }
    delete [] adj_tmp;
    src = new int[eh.size()];
    dst = new int[eh.size()];
    sim = new double[eh.size()];
    num_edges = 0;
    
    for (auto p : eh) {
      Key64 h = p.first;
      eUnhash(h, src[num_edges], dst[num_edges]);
      sim[num_edges] = p.second;
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
  double *sim;
  double **adj_sim;
};

#endif
