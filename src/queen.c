#include <stdio.h>
#include "queen.h"

// Get the neighbor in a specific direction
unsigned int get_neighbor_queen(struct graph_t* v, unsigned int idx, enum dir_t d){
    gsl_spmatrix_uint *csr = v->t;
    for (int k = csr->p[idx]; k < csr->p[idx+1]; k++) {
        unsigned int j = csr->data[k];
         if(j == d){
            return csr->i[k];
         }
    }
    return UINT_MAX;
}
 
 // Checking if the queen is blocked
unsigned int is_queen_blocked(board_t b, player_t player, unsigned int queen_pos){
    gsl_spmatrix_uint *csr = player.graph->t;
    for (int k = csr->p[queen_pos]; k < csr->p[queen_pos+1]; k++) {
        unsigned int j = csr->i[k];
        if(b.pos[j] == EMPTY){
            return 0;
        }             
    }
    return 1;
}

// Choosing randomly which queen to move (It returns the index)
unsigned int choose_random_queen(board_t b, player_t p) { 
    unsigned int i = rand() % p.num_queens;
    if(is_queen_blocked(b, p, p.queens[p.player_id][i]) == 0) {
        return i;
    }
    // If random queen is not free we check next queen
    for(unsigned int i = 0; i < p.num_queens; ++i) {
        if(is_queen_blocked(b, p, p.queens[p.player_id][i]) == 0){
            return i;;
        }
    }
    printf("All queens are blocked\n");
    return 55;
}

// Control for available queens
unsigned int queens_available(board_t b, player_t p){
    for(unsigned int i=0; i<p.num_queens; i++){
        if(is_queen_blocked(b, p, p.queens[p.player_id][i]) == 0){
            return 1;
        }
    }
    return 0;
}

//Returning a random position where to shoot
unsigned int shoot_rand(board_t b, player_t p, unsigned int queen_pos){
    unsigned int avalaible_shoot[p.num_vertices];
    unsigned int size_of_array = 0;

    for(enum dir_t dir= FIRST_DIR; dir <= LAST_DIR; dir++){
        unsigned int neighbor = get_neighbor_queen(p.graph, queen_pos, dir);
        while((neighbor != UINT_MAX) && (b.pos[neighbor] == EMPTY)){
            avalaible_shoot[size_of_array] = neighbor;
            neighbor = get_neighbor_queen(p.graph, neighbor, dir);
            size_of_array++;
        }
    }

    return avalaible_shoot[rand() % size_of_array];
}