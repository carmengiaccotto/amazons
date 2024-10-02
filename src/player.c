#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "queen.h"

char const* get_player_name() {
    return "Finn";
}

// Global board b
board_t b;

// Global player p
player_t p;

void print_board() {
    printf("Board view for player%d:\n",p.player_id);
    unsigned int a = sqrt(p.num_vertices);
    for (unsigned int i = 0; i < p.num_vertices; ++i) {
        if (i % a == 0) {
            printf("\n");
        }
        switch (b.pos[i])
        {
        case EMPTY:
            printf("0 ");
            break;
        case QUEEN:
            printf("1 ");
            break;
        case QUEEN_OPP:
            printf("2 ");
            break;
        case BLOCKED:
            printf("3 ");
            break;
        default:
            break;
        }
    }
    printf("\n\n");
}

// Start to initialize our board 
void initialize_board(){  
    b.pos = calloc(p.num_vertices, sizeof(enum pos_type));
    for(unsigned int k=0; k< p.num_queens; k++){ 
        b.pos[p.queens[p.player_id][k]] = QUEEN;
        b.pos[p.queens[p.player_id_opponent][k]] = QUEEN_OPP;
    }
}

void initialize(unsigned int player_id, struct graph_t* graph,
                unsigned int num_queens, unsigned int* queens[NUM_PLAYERS]){
    p.player_id = player_id;
    p.player_id_opponent = 1 - p.player_id;
    p.num_queens = num_queens;
    p.num_vertices = graph->num_vertices;
    p.graph = graph;
    for(unsigned int i = 0; i < NUM_PLAYERS; i++){
        p.queens[i] = queens[i];
    }
    initialize_board();
}

struct move_t play(struct move_t previous_move){

    struct move_t v = { UINT_MAX, UINT_MAX, UINT_MAX };
    
    if (previous_move.arrow_dst != UINT_MAX || previous_move.queen_dst != UINT_MAX || previous_move.queen_src != UINT_MAX) {
        // Updating the board with previous_move 
        b.pos[previous_move.queen_src] = EMPTY;
        b.pos[previous_move.queen_dst] = QUEEN_OPP;
        b.pos[previous_move.arrow_dst] = BLOCKED;
    }
    // Control on server
    assert(queens_available(b, p) == 1);
    // Choosing randomly which queen to move
    unsigned int queen_index = choose_random_queen(b, p);
    if (queen_index == 55) {
        printf("I have no available queens anymore\n");
        return v;
    }
    // Moving the queen in the first position avalaible, just one step forward
    gsl_spmatrix_uint *csr = p.graph->t;
    unsigned int queen_pos = p.queens[p.player_id][queen_index];
    for (int k = csr->p[queen_pos]; k < csr->p[queen_pos+1]; k++) {
        int j = csr->i[k];
        if(b.pos[j] == EMPTY){
            b.pos[queen_pos] = EMPTY;
            v.queen_src = queen_pos;
            b.pos[j] = QUEEN;
            v.queen_dst = j;
            p.queens[p.player_id][queen_index] = j;
            break;
        }             
    }
    // Shooting randomly in one of the positions avalaible 
    v.arrow_dst = shoot_rand(b, p, v.queen_dst);
    assert(b.pos[v.arrow_dst] == EMPTY);
    b.pos[v.arrow_dst] = BLOCKED;
    print_board();
    return v;
}

void finalize(){
    for(unsigned int i = 0; i<NUM_PLAYERS; ++i) {
        free(p.queens[i]);
    }
    free_graph(p.graph);
    free(b.pos);
    b.pos = NULL;
    printf("%s says thanks for playing and au revoir!\n", get_player_name());
}