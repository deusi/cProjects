#include "main.h"

int main(int argc, char **argv) {
	int num_vertex;
	char cmds[MAXNODE][MAXCHAR];
	if (argc != 2) {
		printf("Incorrect number of arguments...\n");
		printf("Usage: ./proctree input_text_file\n");
	}
	// read file from command line
	char* filename = argv[1];
	FILE* fp = fopen(filename, "r");
	char str[MAXCHAR];
	if(fp == NULL) {
		printf("Fail to open file %s", filename);
		return -1;
	}
	// read number of vertex, which is first line in text file
	fgets(str, MAXCHAR, fp);
	num_vertex = atoi(str);
	// check if empty new line exist after first line
	fgets(str, MAXCHAR, fp);
	if ('\n' != str[0]) {
		printf("Error: cannot find empty new line after number of vertex!\n");
		return 0;
	}
	// record number of command into array
	int cmd_index = 0;
	while(fgets(str, MAXCHAR, fp) != NULL) {
		if('\n' == str[0])
			break;
		else {
			for(int col = 0; col < MAXCHAR; col++)
				cmds[cmd_index][col] = str[col];
			cmd_index++;
		}
	}
	// record the direction of vertices from text file
	int temp_direct_vertex[MAXEDGE][2];
	int node1, node2;
	int num_edge = 0;
	while(fscanf(fp, "%d %d", &node1 , &node2) != EOF) {
		temp_direct_vertex[num_edge][0] = node1;
		temp_direct_vertex[num_edge][1] = node2;
		// test if direction of vertices are correctly added
		num_edge++;
	}
	int direct_vertex[num_edge][2];
	for(int i = 0; i < num_edge; i++) {
		direct_vertex[i][0] = temp_direct_vertex[i][0];
		direct_vertex[i][1] = temp_direct_vertex[i][1];
	}
	fclose(fp);
	// create graph with desired number of nodes
	struct DepGraph* graph =  createGraph(num_vertex);
	// add edges with direction for the graph accroding to
	// the input file
	for(int edge = 0; edge < num_edge; edge++) {
		addEdge(graph, direct_vertex[edge][0], direct_vertex[edge][1]);
	}
 	// write to file first then execute
 	FILE* out = fopen("results.txt", "a");
 	if(out == NULL) {
		printf("Fail to open results.txt");
		return -1;
	}
	DFS(graph, 0, num_vertex, out, cmds);
 	fclose(out);
	return 0;
}