#ifndef _AMAZON_QUEEN_H_
#define _AMAZON_QUEEN_H_

#include "playerext.h"
#include "graph.h"
#include "board.h"
#include "graphext.h"

unsigned int get_neighbor_queen(struct graph_t* v, unsigned int idx, enum dir_t d);

unsigned int is_queen_blocked(board_t b, player_t p, unsigned int queen_pos);

unsigned int choose_random_queen(board_t b, player_t p);

unsigned int queens_available(board_t b, player_t p);

unsigned int shoot_rand(board_t b, player_t p, unsigned int queen_pos);

#endif // _AMAZON_QUEEN_H_