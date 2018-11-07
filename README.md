# WMW

A fast heuristic algorithm for community detection in large-scale complex networks. This is a reference implementation of the algorithm proposed in https://dl.acm.org/citation.cfm?doid=3110025.3110125, https://arxiv.org/abs/1805.01419, https://arxiv.org/abs/1805.12238.

This repository is integral part of the Master's thesis: http://bdigital.unal.edu.co/69933/

# To build run in the command line:

<code>$ make clean</code><br>
<code>$ make</code>

Note: The compiler must be compatible with the C++11 standard.

# Command line options:

<code><b>--quiet -q</b></code> No verbose. <br>
<code><b>--input_file -i</b></code> Input file with the unweighted graph in edge list format<br>
<code><b>--output_file -o</b></code> Output file with the detected communities in format given by parameter -f.<br>
<code><b>--output_format -f</b></code> Output format of the detected communities: 0 for node community per line. 1 for community per line<br>
<code><b>--gml -g</b></code> Output graph in GML format with membership (Useful for visual exploratory analysis in Gephi). Only works if the -O option is not specified<br>
<code><b>--size_distri_file -z</b></code> Output community size distribution (Communities of size X vs frequency).<br>
<code><b>--edge_sims_file -s</b></code> Output the dynamic structural similarity values for each edge in the graph (node1 node2 similarity).<br>
<code><b>--memb_dist_file -m</b></code> Output the membership distribution (nodes with X communities vs frequency).<br>
<code><b>--node_numbering -n</b></code> Index of the first node in the graph. Default to 0. Common values {0, 1}.<br>
<code><b>  --asynchronous_wmw -a</b></code> Run the asynchronous implementation of the algorithm. If this option is not specified, the synchronous implementation is used.<br>
<code><b>--community_def -C</b></code> Community Definition. 0 for Weak definition, 1 for Most Weak definition. Default value 0.<br>
<code><b>--min_comm_size -K</b></code> Minimum community size in the result. Default value 3.<br>
<code><b>--dss_iterations -I</b></code> Number of iterations to perform by the Dynamic Structural Similarity.<br>
<code><b>--overlap -O</b></code> Detect overlapping communities: 0 for fuzzy communities. 1 for crisp communities with threshold -T.<br>
<code><b>--crisp_threshold -T</b></code> Threshold used to generate the crisp overlapping communities. If this option is not specified, a threshold is automatically chosen by the algorithm. Common values {0.001, 0.005, 0.01, 0.02, 0.03, 0.04, 0.05}<br>

# Example of Disjoint Community Detection:

<code>./master -i input_graph.txt -o output_memberships.txt -f 0 -g output_gml.gml -C 0 -K 3 -I 2</code>


# Example of Overlapping Community Detection:

<code>./master -i input_graph.txt -o output_communities.txt -f 1 -C 0 -K 3 -I 2 -O 1 -T 0.05</code>

# NOTES:

Self-loops are removed.<br>
Zero degree vertices are skipped.<br>
The input graph is considered undirected and unweighted.<br>

<i><b>This code is not intended for production.</b></i>
