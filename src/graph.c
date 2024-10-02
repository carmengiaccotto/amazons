#include "graph.h"
#include "graphext.h"
#include <string.h>
#include <assert.h>
#define DONUT 3
#define CLOVER 5
#define HUIT 4

unsigned int get_neighbor(unsigned int m, unsigned int idx, enum dir_t d){
    if ((idx < m) && ((d == DIR_NE) || (d == DIR_NORTH) || (d == DIR_NW)))
        return UINT_MAX;
    if ((idx % m == 0) && ((d == DIR_NW) || (d == DIR_WEST) || (d == DIR_SW)))
        return UINT_MAX;
    if ((idx % m == m - 1) && ((d == DIR_NE) || (d == DIR_EAST) || (d == DIR_SE)))
        return UINT_MAX;
    if ((idx >= m * (m - 1)) && ((d == DIR_SW) || (d == DIR_SOUTH) || (d == DIR_SE)))
        return UINT_MAX;
    else
    {
        switch (d)
        {
        case DIR_NW:
            return (idx - m - 1);
        case DIR_NORTH:
            return (idx - m);
        case DIR_NE:
            return (idx - m + 1);
        case DIR_WEST:
            return (idx - 1);
        case DIR_EAST:
            return (idx + 1);
        case DIR_SW:
            return (idx + m - 1);
        case DIR_SOUTH:
            return (idx + m);
        case DIR_SE:
            return (idx + m + 1);
        default:
            return UINT_MAX;
        }
    }
}

unsigned int get_direction(unsigned int m, unsigned int idx, unsigned int idx_2){

    if((idx - m - 1) == idx_2)
        return DIR_NW;
    if((idx - m) == idx_2)
        return DIR_NORTH;
    if((idx - m + 1) == idx_2)
        return DIR_NE;
    if((idx - 1) == idx_2)
        return DIR_WEST;
    if((idx + 1) == idx_2)
        return DIR_EAST;
    if((idx + m - 1) == idx_2)
        return DIR_SW;
    if((idx + m) == idx_2)
        return DIR_SOUTH;
    if((idx + m + 1) == idx_2)
        return DIR_SE;
    
    return NO_DIR;

}

unsigned int index_is_good(unsigned int idx, unsigned int t, unsigned int m){
    unsigned int row = idx / m;
    unsigned int col = idx % m;

    if(t == 1){
        if(col >= (m/DONUT) && col < ((2*m)/DONUT) && row >= (m/DONUT) && row < ((2*m)/DONUT)){
            return 0;
        }else{
            return 1;
        }
    }else if(t == 2){
        if(((col >= (m/CLOVER) && col < ((2 * m)/CLOVER)) || (col >= ((3 * m)/CLOVER) && col < ((4 * m)/CLOVER))) 
            && ((row >= (m/CLOVER) && row < ((2 * m)/CLOVER)) || (row >= ((3 * m)/CLOVER) && row < ((4 * m)/CLOVER)))){
            return 0;
        }else{
            return 1;
        }
    }else if(t == 3){
        if((col >= (m/HUIT) && col < ((2 * m)/HUIT) && row >= ((2 * m)/HUIT) && row < ((3 * m)/HUIT)) 
            || (row >= (m/HUIT) && row < ((2 * m)/HUIT) && col >= ((2 * m)/HUIT) && col < ((3 * m)/HUIT))){
            return 0;
        }else{
            return 1;
        }
    }else{
        return 1;
    }
}

struct graph_t* generate_graph(unsigned int m, unsigned int t){
   struct graph_t* graph = malloc(sizeof(struct graph_t));
   
    graph->num_vertices = m * m;

    graph->t = gsl_spmatrix_uint_alloc(graph->num_vertices, graph->num_vertices);

    for(unsigned int i=0; i < m * m; i++){
        if ( !index_is_good(i, t, m) )
            continue;
        for(enum dir_t dir= FIRST_DIR; dir <= LAST_DIR; dir++){
            unsigned int neighbor = get_neighbor(m, i, dir);
            if((neighbor != UINT_MAX) && index_is_good(neighbor, t, m)){ 
                gsl_spmatrix_uint_set(graph->t, i, neighbor, get_direction(m, i, neighbor));
            }
        }
    }

   // converting the matrix to the CSR format
    gsl_spmatrix_uint *csr = gsl_spmatrix_uint_compress(graph->t, GSL_SPMATRIX_CSR);
    assert(strcmp(gsl_spmatrix_uint_type(csr), "CSR") == 0);
    gsl_spmatrix_uint_free(graph->t);
    graph->t = csr;

   return graph;
}

struct graph_t* copy_graph(struct graph_t* graph) {
    struct graph_t* copy = malloc(sizeof(struct graph_t));
    if (copy == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for graph copy\n");
        return NULL;
    }
    // Copy number of vertices
    copy->num_vertices = graph->num_vertices;
    // Allocate memory for copy of the sparse matrix
    copy->t = gsl_spmatrix_uint_alloc(graph->num_vertices, graph->num_vertices);
    if (copy->t == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for graph copy\n");
        free(copy);
        return NULL;
    }
    // Copy the values from the original sparse matrix to the copy
    gsl_spmatrix_uint_memcpy(copy->t, graph->t);
    return copy;
}

void free_graph(struct graph_t* graph) {
    gsl_spmatrix_uint_free(graph->t);
    free(graph);
    graph = NULL;
}

