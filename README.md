# HAMUHI

A reference implementation of the algorithm proposed in https://arxiv.org/abs/1707.02362

# To build run in the command line:

$ make clean<br>
$ make

# Command line options:

<code><b>-i</b></code> Input file with the graph in edge list format<br>
<code><b>-o</b></code> Output file for membership for each vertex in the graph.<br>
<code><b>-g</b></code> Output graph in format GML with vertices membership (Useful for visual exploratory analysis in Gephi).<br>
<code><b>-s</b></code> Output community size distribution.<br>
<code><b>-c</b></code> Community Definition. 0 for Weak definition, 1 for Weakest definition. Default value 0.<br>
<code><b>-k</b></code> Minimum community size in the result. Default value 2.<br>

# Running example:

<code>./hamuhi -i input_graph -o output_membership -g output_gml -s output_size_dist -c 0 -k 2</code>

# NOTES:

Self-loops are skipped.<br>
The vertices are numbered from 0 to N-1, where N-1 is the highest identifier in the input graph.<br>
Zero degree vertices are skipped.<br>
The input graph is considered undirected.<br>

<i><b>This code is not intended for production.</b></i>
