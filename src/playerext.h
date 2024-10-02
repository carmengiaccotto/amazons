#ifndef _AMAZON_PLAYEREXT_H_
#define _AMAZON_PLAYEREXT_H_

#define NUM_PLAYERS 2

typedef struct player_s {
    unsigned int player_id;
    unsigned int player_id_opponent;
    unsigned int num_vertices;
    struct graph_t *graph;
    unsigned int num_queens;
    unsigned int* queens[NUM_PLAYERS]; 
} player_t;

#endif



