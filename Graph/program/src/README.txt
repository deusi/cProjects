/*test machine: csel-kh4240-[01]
* name: Denis Rybkin
* x500: rybki001 */

Purpose:
The purpose of this program is to run command lines in nodes of a graph where each
node represent one command line with Depth First Search using fork/exec/wait. The graph is
created by a input file with vertex number, command line strings and direction of vertices
in graph. The command line outputs will show in terminal and the child id, parent id and
command line stirng will be wrote into file results.txt.

How to Compile File:
type "make" in src directory of the project will compile all required files for the project
and the executable depGraph will be created.

The project can be executed with typing ./depGraph [filename] in terminal, where [filename]
is the file that provides all required informaiton about a graph.

What my program does:
In main file, I read number of vertices and command line strings from file. Then, a graph is
created based on adjacency list with function createGraph in graph.c. Last, DFS function is
called to traverse the graph. When DFS function is called, I split the command line strings
to words into parm_list such that execvp can use it as argument. Also, each time a node is
visited, process is forked and exec in DFS function. The program has a known bug that even
though the execution order is correct, each forked process always has same parent id. Hope
TA reviews can give some intuitions about how can I solve this bug.
