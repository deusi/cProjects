#include "graph.h"

struct AdjListNode* newAdjListNode(int dest) {
	struct AdjListNode* newNode = (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
	newNode->dest = dest;
	newNode->next = NULL;
	return newNode;
}

// function that creates a graph of V vertices
struct DepGraph* createGraph(int V) {
	struct DepGraph* graph = (struct DepGraph*) malloc(sizeof(struct DepGraph));
	graph->V = V;
	graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));
	graph->visited = (int*) malloc(V * sizeof(int));
	// dfs_arr = calloc(V, sizeof(int));
	// initialize head pointer for all vertices
	for (int i = 0; i < V; ++i) {
		graph->array[i].head = NULL;
		graph->visited[i] = 0;
	}
	return graph;
}

// Adds an edge to an directed graph
void addEdge(struct DepGraph* graph, int src, int dest) {
	// add edge to last element of linkedlist
    struct AdjListNode* new_node = newAdjListNode(dest);
    struct AdjListNode* cur_node = graph->array[src].head;
    while(cur_node != NULL && cur_node->next != NULL) {
    	cur_node = cur_node->next;
    }
    if(cur_node != NULL)
    	cur_node->next = new_node;
    else
    	graph->array[src].head = new_node;
}

void DFS(struct DepGraph* graph, int vertex, int num_vertex, FILE* out, char cmds[MAXNODE][MAXCHAR]) {
	// 550
	char str_cmd[MAXCHAR];
 	strcpy(str_cmd, cmds[vertex]);
 	// 10 x 50
 	char new_str[MAXARGS][MAXCHARS] = {0};
 	int word_idx = 0;
 	int char_idx = 0;
 	// get commands of current nodes separated by space
 	for(int i = 0; i < (strlen(str_cmd)); i++) {
 		// check if end line found
 		if(str_cmd[i] == '\n') {
 			//new_str[word_idx][char_idx] = '\n';
 			break;
 		}
 		// check if space or null found
 		else if(str_cmd[i] == ' ' || str_cmd[i] == '\0') {
 			new_str[word_idx][char_idx] = '\0';
 			// go next word
 			word_idx++;
 			// reset char index for next word
 			char_idx = 0;
 		}
 		else {
 			// assign char to new string
 			new_str[word_idx][char_idx] = str_cmd[i];
 			char_idx++;
 		}
 	}
	char* path = new_str[0];
 	char* parm_list[word_idx+2];
 	for(int i = 0; i < word_idx+1; i++) {
 		parm_list[i] = new_str[i];
 	}
 	// set last element of parm_list to terminated string
 	parm_list[word_idx+1] = NULL;
	struct AdjListNode* cur_node;
	cur_node = graph->array[vertex].head;
	graph->visited[vertex] = 1;
	while(cur_node != NULL) {
		vertex = cur_node->dest;
		if(!graph->visited[vertex]){
			DFS(graph, vertex, num_vertex, out, cmds);
		}
		cur_node = cur_node->next;
	}
	//fork recursively as DFS visiting nodes
	pid_t child_pid, wpid;
	int status = 0;
	if((child_pid = fork()) == 0) {
		// child execution
		FILE* fout = fopen("results.txt", "a");
	 	if(out == NULL) {
			printf("Fail to open results.txt");
		}
		pid_t c_pid = getpid();
		pid_t p_pid = getppid();
		fprintf(fout, "%ld %ld %s\n", (long)c_pid, (long)p_pid, str_cmd);
		fclose(fout);
 		execvp(path, parm_list);
		printf("failed to run the program in child process\n");
		exit(-1);
	}
	while((wpid=waitpid(-1,&status,0))!= -1);
}
