#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/player.h"
#include "../src/graphext.h"
#include "../src/graph.h"

// Test for creating graph
void test_creating_graph() {
    unsigned int size = 8;
    unsigned int shape = 0;
    struct graph_t* v = generate_graph(size, shape);
    assert(v->t->size1 == size*size);
    if (v->t->size1 == size*size) {
        printf("Test for creating graph PASSED!\n");
    }
    else {
        printf("Test for creating graph FAILED!\n");
    }
    free_graph(v);

}

// Test for making a copy of graph
void test_copy_and_free_graph() {
    struct graph_t* graph = generate_graph(4, 0);
    struct graph_t* graph_copy = generate_graph(4, 0);
    if(graph->t->size1 == graph_copy->t->size1) {
        printf("Test for copying graph PASSED!\n");
    }
    else {
        printf("Test for copying graph FAILED!\n");
    }
    free_graph(graph);
    free_graph(graph_copy);
    if (graph == NULL && graph_copy == NULL) {
        printf("Test for free graph PASSED!\n");
    }
    else {
        printf("Test for free graph FAILED!\n");
    }
}

// Test for get neighbor
void test_get_neighbor() {
    // West neigbor of 12 should be 11 in standard field.
    unsigned int neigbhor = get_neighbor(8, 12, DIR_WEST);
    if (neigbhor == 11) {
        printf("Test for get neigbor PASSED!\n");
    }
    else {
        printf("Test for get neighbor FAILED!\n");
    }
}

// Test for get direction.
void test_get_direction() {
    unsigned int dir = get_direction(8, 12, 11);
    if (dir == DIR_WEST) {
        printf("Test for get direction PASSED!\n");
    }
    else {
        printf("Test for get direction FAILED!\n");
    }
}

void test_switch_players() {
    unsigned int current_player = 0;
    unsigned int next_player = 1;
    current_player = 1-current_player;
    next_player = 1-next_player;
    if (current_player == 1 && next_player == 0) {
        printf("Test for switching players PASSED!\n");
    }
    else {
        printf("Test for switching players FAILED!\n");
    }
}

// Simple test function
void test_function() {
    printf("THE TEST IS WORKING!!\n");
}

// The main function for the test.
int main() {
    test_function();
    test_creating_graph();
    test_copy_and_free_graph();
    test_get_neighbor();
    test_get_direction();
    test_switch_players();
    return 0;
}