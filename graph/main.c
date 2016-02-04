#include <stdio.h>

enum {
    G_PROPERTY = 'p',
    G_EDGE = 'e',
    BUF_SIZE = 64
};

struct node {
    unsigned id;
    struct node *next;
};

struct graph {
    unsigned n_nodes;
    struct {
        unsigned length;
        struct node *first;
        struct node *last;
    } *nodes;
};

/**
 * allocate memory for a graph and initialize it
 * 'nodes' is not initialized
 */
struct graph *init_graph(void);

/**
 * add an edge from 'from' to 'to' to the graph 'g'
 */
int graph_add_edge(struct graph *g, unsigned from, unsigned to);

int parse_graph(FILE *input, struct graph *g);
int parse_propertyline(char line[], struct graph *g);
int parse_edge(char line[], struct graph *g);

int main(void) {
    /* Real hackers implement a -f option */
    FILE *input = fopen("sample.col", "r");

    struct graph *g = init_graph();
    parse_graph(input, g);

    fclose(input);
    return 0;
}

int parse_graph(FILE *input, struct graph *g) {
    if (input == NULL) {
        return 1;
    }

    char line[BUF_SIZE];

    while (fgets(line, sizeof line, input) != NULL) {
        switch (line[0]) {
            case G_PROPERTY:
                parse_propertyline(line, g);
                break;
            case G_EDGE:
                parse_edge(line, g);
                break;
        }
    }

    return 0;
}

int parse_edge(char line[], struct graph *g) {
    unsigned from;
    unsigned to;
    if (sscanf(line, "e %u %u", &from, &to) < 2) {
        return 1;
    }
    printf("Edge from %u to %u\n", from, to);
    return 0;
}

int parse_propertyline(char line[], struct graph *g) {
    unsigned n_nodes;
    if (sscanf(line, "p edge %u %*u", &n_nodes) < 1) {
        return 1;
    }
    printf("Number of nodes: %u\n", n_nodes);
    return 0;
}
