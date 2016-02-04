#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/* print a graph */
void print_graph(struct graph *g);

/* print a path */
int depth_first_search(struct graph *g, unsigned from, unsigned to);

int parse_graph(FILE *input, struct graph *g);
int parse_propertyline(char line[], struct graph *g);
int parse_edge(char line[], struct graph *g);

/* interactive mode */
int interactive(struct graph *g);

int main(void) {
    /* Real hackers implement a -f option */
    FILE *input = fopen("sample.col", "r");

    struct graph *g = init_graph();
    parse_graph(input, g);

    while (interactive(g))
        ; /* PASS */

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
    return graph_add_edge(g, from, to);
}

int parse_propertyline(char line[], struct graph *g) {
    unsigned n_nodes;
    if (sscanf(line, "p edge %u %*u", &n_nodes) < 1) {
        return 1;
    }

    /* Don't set up the graph twice */
    if (g->n_nodes != 0) {
        puts("Warning: file contains two property lines");
        return 2;
    }

    g->nodes = calloc(n_nodes, sizeof *g->nodes);
    if (g->nodes == NULL) {
        perror("parse_propertyline");
        exit(EXIT_FAILURE);
    }
    g->n_nodes = n_nodes;
    return 0;
}

struct graph *init_graph(void) {
    struct graph *g = malloc(sizeof *g);
    if (g == NULL) {
        perror("init_graph");
        exit(EXIT_FAILURE);
    }
    g->n_nodes = 0;
    g->nodes = NULL;
    return g;
}

int graph_add_edge(struct graph *g, unsigned from, unsigned to) {
    /* Is the node actually in the graph? */
    if (g->n_nodes < from || g->n_nodes < to) {
        printf("Warning: invalid edge (%u, %u)\n", from, to);
        return 1;
    }

    /* Prevent redundant edges */
    for (struct node *n = g->nodes[from-1].first; n != NULL; n = n->next) {
        if (n->id == to) {
            printf("Warning: redundant edge (%u, %u)\n", from, to);
            return 1;
        }
    }

    struct node *n = malloc(sizeof *n);
    if (n == NULL) {
        perror("graph_add_edge");
        exit(EXIT_FAILURE);
    }
    n->id = to;
    n->next = NULL;
    if (g->nodes[from-1].length == 0) {
        g->nodes[from-1].first = n;
        g->nodes[from-1].last = n;
    } else {
        g->nodes[from-1].last->next = n;
        g->nodes[from-1].last = n;
    }
    g->nodes[from-1].length += 1;
    return 0;
}

void print_graph(struct graph *g) {
    printf("===\nGraph with %u nodes:\n{", g->n_nodes);
    for (unsigned from = 1; from <= g->n_nodes; ++from) {
        for (struct node *n = g->nodes[from-1].first;
                n != NULL;
                n = n->next) {
            printf("\t(%u, %u)\n", from, n->id);
        }
    }
    printf("}\n");
}

int depth_first_search(struct graph *g, unsigned from, unsigned to) {
    int visited[g->n_nodes];
    memset(visited, 0, sizeof visited);

    unsigned parent[g->n_nodes];
    memset(parent, 0, sizeof parent);

    unsigned stack[g->n_nodes];
    unsigned stackpointer = 0;

    stack[stackpointer++] = from;
    visited[from-1] = 1;

    while (stackpointer > 0) {
        unsigned current = stack[--stackpointer];
        if (current == to) {
            printf("%u", current);
            for (unsigned n = current; parent[n-1] != 0; n = parent[n-1]) {
                printf("<=%u", parent[n-1]);
            }
            puts("");
            return 0;
        }
        for (struct node *n = g->nodes[current-1].first;
                n != NULL;
                n = n->next) {
            if (!visited[n->id-1]) {
                stack[stackpointer++] = n->id;
                visited[n->id-1] = 1;
                parent[n->id-1] = current;
            }
        }
    }
    printf("No path from %u to %u.\n", from, to);
    return 1;
}

int interactive(struct graph *g) {
    char buffer[BUF_SIZE];
    puts("Enter a command:");
    fgets(buffer, sizeof buffer, stdin);
    switch (buffer[0]) {
        case 'p': print_graph(g); return 1;
        case 'q': return 0;
        case 's': {
                      unsigned from;
                      unsigned to;
                      if (sscanf(buffer, "s %u %u", &from, &to) == 2) {
                          depth_first_search(g, from, to);
                      }
                      return 1;
                  }
        default: return 1;
    }
}
