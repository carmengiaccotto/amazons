#ifndef _AMAZON_GRAPHEXT_H_
#define _AMAZON_GRAPHEXT_H_

#include "dir.h"

unsigned int get_neighbor(unsigned int m, unsigned int idx, enum dir_t d);

unsigned int get_direction(unsigned int m, unsigned int idx, unsigned int idx_2);

unsigned int index_is_good(unsigned int idx, unsigned int t, unsigned int m);

struct graph_t* generate_graph(unsigned int m, unsigned int t);

struct graph_t* copy_graph(struct graph_t* graph);

void free_graph(struct graph_t* graph);

#endif //_AMAZON_GRAPHEXT_H_