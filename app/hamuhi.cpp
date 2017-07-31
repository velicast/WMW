#include "lib/Graph.h"
#include "lib/CosineSimilarity.h"
#include "lib/Modularity.h"
#include "lib/DisjointSet.h"
#include "lib/Clustering.h"
#include "lib/HAMUHI.h"
#include <getopt.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unordered_map>

using namespace std;

int quiet = 0;
int gml_format = 0;
int min_size = 2;
int comm_def = WEAK_CLUSTER;
int map_nodes = 0;
int *v_map;
fstream szstats;
fstream gml;
clock_t begin_time;

void parseArgs(int argc, char **argv) {
  
  struct option long_options[] = {
    {"quiet",   no_argument,        0, 'q'},
    {"gml",     required_argument,  0, 'g'},
    {"input",   required_argument,  0, 'i'},
    {"output",  required_argument,  0, 'o'},
    {"minsize", required_argument,  0, 'k'},
    {"szstats", required_argument,  0, 's'},
    {"commdef", required_argument,  0, 'c'},
    {0, 0, 0, 0}
  };
  int cmd, option_index = 0;

  while ((cmd = getopt_long (argc, argv, "qg:i:o:k:c:s:", long_options, &option_index)) != -1) {
    if (cmd == 'q') {
      quiet = 1;
      freopen("/dev/null", "w", stderr);
    }
    else if (cmd == 'm') {
      map_nodes = 1;
    }
    else if (cmd == 'g') {
      gml.open(optarg, fstream::out | fstream::trunc);

      if (!gml.is_open()) {
        cerr << "Failed to open gml file " << optarg << endl;
        exit(-1);
      }
    }
    else if (cmd == 'i') {
      if (freopen(optarg, "r", stdin) == NULL) {
        cerr << "Failed to open input file " << optarg << endl;
        exit(-1);
      }
    }
    else if (cmd == 'o') {
      if (freopen(optarg, "w", stdout) == NULL) {
        cerr << "Failed to open output file " << optarg << endl;
        exit(-1);
      }
    }
    else if (cmd == 'k') {
      min_size = atoi(optarg);
      if (min_size < 1) {
        cerr << "Invalid option value for " << char(cmd) << ": " << optarg << endl;
        exit(-1);
      }
    }
    else if (cmd == 'c') {
      comm_def = atoi(optarg);
      if (comm_def < 0 || comm_def > 2) {
        cerr << "Invalid option value for " << char(cmd) << ": " << optarg << endl;
        exit(-1);
      }
    }
    else if (cmd == 's') {
      szstats.open(optarg, fstream::out | fstream::trunc);

      if (!szstats.is_open()) {
        cerr << "Failed to open szstats file " << optarg << endl;
        exit(-1);
      }
    }
    else if (cmd == '?') {
      cerr << "Please read the manual pages" << endl;
      exit(-1);
    }
  }
}

void startTimer(string msg) {

  cerr << msg;
  cerr.flush();
  begin_time = clock();
}

void endTimer() {
   cerr << double(clock()-begin_time)/CLOCKS_PER_SEC << " sec(s)" << endl;
}

Graph *readGraph() {
 
  int iu, iv, n = 0, i = 0;
  unsigned long long u, v;
  unordered_map<unsigned long long, int> u_map;
  vector<int> vu, vv;

  while (cin >> u >> v) {
    if (!u_map.count(u)) {
      u_map[u] = i;
      iu = i++;
    } else {
      iu = u_map[u];
    }
    if (!u_map.count(v)) {
      u_map[v] = i;
      iv = i++;
    } else {
      iv = u_map[v];
    }
    vu.push_back(iu);
    vv.push_back(iv);
    n = max(n, max(iu, iv));
  }
  v_map = new int[n+1];

  for (auto it = u_map.begin(); it != u_map.end(); it++) {
    v_map[it->second] = it->first;
  }
  u_map.clear();
  Graph *g = new Graph(n+1);
  g->addEdges(vu, vv);
  vu.clear();
  vv.clear();
  return g;
}

void outGML(Clustering &clustering) {
  
  gml << fixed << setprecision(2);
  gml << "graph [\n";
  gml << "directed 0\n";
  
  Graph &g = *(clustering.graph);

  for (int i = 0; i < g.num_vertices; i++) {
    gml << "node [\n";
    gml << "id " << v_map[i] << "\n";
    gml << "co " << v_map[clustering.getMembership(i)] << "\n";
    gml << "]\n";
  }
  for (int i = 0; i < g.num_edges; i++) {
    gml << "edge [\n";
    gml << "source " << v_map[g.src[i]] << "\n";
    gml << "target " << v_map[g.dst[i]] << "\n";
    gml << "]\n";
  }
  gml << "]\n";
}

void outMembership(Clustering &clustering) {
  
  for (int i = 0; i < clustering.graph->num_vertices; i++) {
    cout << v_map[i] << "\t" << v_map[clustering.getMembership(i)] << "\n";
  }
}

void outSizeStatistics(Clustering &clustering) {

  int min_sz = 1e9, max_sz = 0, comms = 0, avg_sz = 0;
  int *freq = new int[clustering.graph->num_vertices+1];
  memset(freq, 0, clustering.graph->num_vertices*sizeof(int));

  for (DSSetIterator it = clustering.getClusters(); it.hasNext();) {
    int sz = clustering.getClusterSize(it.next());
    freq[sz] += sz > 0;

    if (sz >= min_size) {
      min_sz = min(min_sz, sz);
      max_sz = max(max_sz, sz);
      avg_sz += sz;
      comms++;
    }
  }
  szstats << "community_size\tfrequency\n";

  for (int i = 0; i < clustering.graph->num_vertices; i++) {
    if (freq[i]) {
      szstats << i << "\t" << freq[i] << "\n";
    }
  }
  delete [] freq;

  int w = 15;
  cerr << string(5*w, '-') << endl;
  cerr << setw(w) << "communities" << setw(w) << "fit size" << setw(w) << "min size" << setw(w) << "max size" << setw(w) << "avg size" << endl;
  cerr << setw(w) << clustering.getNumClusters() << setw(w) << setw(w) << comms << setw(w) << min_sz << setw(w) << max_sz
       << setw(w) << setprecision(2) << 1.*avg_sz/comms << endl;
}

int main(int argc, char **argv) {
  //ios_base::sync_with_stdio(false);
  //cout.tie(0);
  //cin.tie(0);
  parseArgs(argc, argv);
  cerr << fixed << setprecision(5);

  startTimer("*** Loading graph... ");
    Graph *g = readGraph();
  endTimer();
  
  cerr << "- Vertices: " << g->num_vertices << endl << "- Edges: " << g->num_edges << endl;
  
  startTimer("*** Running HAMUHI... ");
    Clustering *clustering = new Clustering(g);
    CosineSimilarity::unweighted(*g);
  
    if (comm_def == WEAK_CLUSTER) {
      HAMUHI::weakCluster(*clustering);
    }
    else if (comm_def == WEAKEST_CLUSTER) {
      HAMUHI::weakestCluster(*clustering);
    }
    HAMUHI::minClusterSize(*clustering, min_size);
  endTimer();
  
  int *guess = new int[g->num_vertices];

  for (int i = 0; i < g->num_vertices; i++) {
    guess[i] = clustering->getMembership(i);
  }
  cerr << fixed << setprecision(3) << "- Modularty: " << Modularity::compute(*g, guess) << endl;

  delete [] guess;
  
  outMembership(*clustering);
  
  if (szstats.is_open()) {
    outSizeStatistics(*clustering);
    szstats.close();    
  }
  if (gml.is_open()) {
    outGML(*clustering);
    gml.close();    
  }
  delete [] v_map;
  delete g;
  delete clustering;
  return 0;
}
