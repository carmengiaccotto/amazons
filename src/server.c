// #include "externserver.h"
#include "graphext.h"
// In player.h are already included graph.h and move.h
#include "player.h"
#include "queen.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <dlfcn.h>
#include <unistd.h>
// Need math.h for initializing positions of the queens (round())
#include <math.h>
#include <string.h>
#include <time.h>
#include <assert.h>

// A struct for all generel values of the game
typedef struct game_values_s {
    // m is the width of the game board.
    unsigned int m;
    // t is the shape of the game board (0=square; 1=donut; 2=trèfle; 3=eight)
    unsigned int t;
    unsigned int player1_id;
    unsigned int player2_id;
    // Positions of the queens
    unsigned int *queens[NUM_PLAYERS];
    // Copy of queens for player 1 and 2.
    unsigned int *queens_p1[NUM_PLAYERS];
    unsigned int *queens_p2[NUM_PLAYERS]; 
    // Number of queens
    unsigned int num_queens;
    // Positions of blocked positions (1 = blocked; 0 = free)
    unsigned int *blocked_pos;
    // The graph of the game
    struct graph_t* graph;
    struct graph_t* graph_p1;
    struct graph_t* graph_p2;
    // the struct move
    struct move_t move;
    struct move_t previous_move;
    // Current and next player
    unsigned int current_player;
    unsigned int next_player;
} game_values_t;

// A struct for a player
typedef struct players_s {
    // Player ID
    unsigned int player_id;
    // Filename
    char* filename;
    // handle is the pointer of the dynamic lib.
    void *handle;
    // Safe the name of a player.
    const char *(*player_name)();
    // Pointer of the function 'initialize'.
    void (*initialize)(unsigned int player_id, struct graph_t* graph, unsigned int num_queens, unsigned int* queens[NUM_PLAYERS]);
    // Pointer of the function 'play'.
    struct move_t (*play)(struct move_t previous_move);
    // struct move_t (*play)(void*, struct move_t);
    // Pointer of the function 'finalize'.
    void (*finalize)();
    // For a possible error message.
    char *error;
} players_t;

// Global values for the game
game_values_t v;

// Global players 1 and 2.
players_t player1;
players_t player2;


void def_game(int argc, char *argv[]) {
    // m is the width of the game board. 
    int mflag = 8;
    // t allows to specify the shape of the game board.
    // 'c' is the standard value
    char tflag = 'c';
    int opt;
    while ((opt = getopt(argc, argv, "m:t:")) != -1) {
        switch (opt) {
            case 'm':
                mflag = atoi(optarg);
                break;
            case 't':
                tflag = optarg[0];
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (optind > argc-2) {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }

    player1.filename = argv[optind];
    player2.filename = argv[optind+1];
    // Defining the struct game_values
    v.m = mflag;
    if (v.m < 10) {
        v.num_queens = 4;
    }
    else {
        v.num_queens = 8;
    }
    switch (tflag) {
    case 'c':
        v.t = 0;
        break;
    case 'd':
        v.t = 1;
        if (v.m%3 != 0) {
            printf("*******************************************************************\n");
            printf("ERROR: Invalid size for donut shape! Size should be divisible by 3.\n");
            printf("*******************************************************************\n");
            assert(v.m%3 == 0);
        }
        break;
    case 't':
        v.t = 2;
        if (v.m%5 != 0) {
            printf("*******************************************************************\n");
            printf("ERROR: Invalid size for trèfle shape! Size should be divisible by 5.\n");
            printf("*******************************************************************\n");
            assert(v.m%5 == 0);
        }
        break;
    case '8':
        v.t = 3;
        if (v.m%4 != 0) {
            printf("*******************************************************************\n");
            printf("ERROR: Invalid size for eight shape! Size should be divisible by 4.\n");
            printf("*******************************************************************\n");
            assert(v.m%4 == 0);
        }
        break;
    default:
        printf("SHAPE ERROR!!\n");
        break;
    }
    v.player1_id = 0;
    v.player2_id = 1;
}

void load_player(players_t *player) {
    // Open the dynamic library and handle errors
    player->handle = dlopen(player->filename, RTLD_LAZY);
    if (!player->handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    dlerror();    /* Clear any existing error */

    // Load necessary functions and store them in the player struct
    player->player_name = (const char *(*)()) dlsym(player->handle, "get_player_name");
    if (dlerror() != NULL) {
        fprintf(stderr, "Error loading player_name function: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    player->initialize = (void (*)()) dlsym(player->handle, "initialize");
    if (dlerror() != NULL) {
        fprintf(stderr, "Error loading initialize function: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    player->play = (struct move_t (*)()) dlsym(player->handle, "play");
    if (dlerror() != NULL) {
        fprintf(stderr, "Error loading play function: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    player->finalize = (void (*)()) dlsym(player->handle, "finalize");if (dlerror() != NULL) {
        fprintf(stderr, "Error loading finalize function: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    player->error = dlerror();
    if (player->error != NULL) {
        fprintf(stderr, "%s\n", player->error);
        exit(EXIT_FAILURE);
    }
}

// Create the two players and store all the information in a struct.
void players_information() {
    // First open and save datas of player1.
    load_player(&player1);
    player1.player_id = 0;
    printf("%s\n", player1.player_name());
    // Open and save datas of player2.
    load_player(&player2);
    player1.player_id = 1;
    printf("%s\n", player2.player_name());
}

// Initialize the positions for the queens 
void initialize_position_queens() {
    unsigned int max_pos = v.m * v.m;
    unsigned int q1 = round(1.0/3*(v.m-1));   // ex: 1.0/3*4=1
    unsigned int q2 = round(2.0/3*(v.m-1));   // ex: 2.0/3*4=3
    unsigned int a = 0;
    unsigned int b = 0;
    // First no position is blocked, so everything is zero
    v.blocked_pos = calloc(v.graph->num_vertices, sizeof(unsigned int));
    v.queens[0] = malloc( v.num_queens * sizeof(unsigned int) );
    v.queens[1] = malloc( v.num_queens * sizeof(unsigned int) );
    v.queens_p1[0] = malloc( v.num_queens * sizeof(unsigned int) );
    v.queens_p1[1] = malloc( v.num_queens * sizeof(unsigned int) );
    v.queens_p2[0] = malloc( v.num_queens * sizeof(unsigned int) );
    v.queens_p2[1] = malloc( v.num_queens * sizeof(unsigned int) );
    for (unsigned int i = 0; i < max_pos; ++i) {
        unsigned int row = i / v.m;
        unsigned int col = i % v.m;
        if (row == 0 && (col == q1 || col == q2)) {
            v.queens[0][a] = i;
            v.queens_p1[0][a] = i;
            v.queens_p2[0][a] = i;
            v.blocked_pos[i] = 1;
            ++a;
        }
        else if ((col == 0 || col == v.m-1) && row == q1) {
            v.queens[0][a] = i;
            v.queens_p1[0][a] = i;
            v.queens_p2[0][a] = i;
            v.blocked_pos[i] = 1;
            ++a;
        }
        else if (row == v.m-1 && (col == q1 || col == q2)) {;
            v.queens[1][b] = i;
            v.queens_p1[1][b] = i;
            v.queens_p2[1][b] = i;
            ++b;
            v.blocked_pos[i] = 1;
        }
        else if ((col == 0 || col == v.m-1) && row == q2) {
            v.queens[1][b] = i;
            v.queens_p1[1][b] = i;
            v.queens_p2[1][b] = i;
            ++b;
            v.blocked_pos[i] = 1;
        }
    }
    for (unsigned int i = 0; i<v.num_queens;++i) {
        printf("P1 Q%d = %d\n",i,v.queens[0][i]);
        printf("P2 Q%d = %d\n",i,v.queens[1][i]);
    }
}

// Initialize the positions for the queens 
void initialize_position_queens_eight() {
    unsigned int max_pos = v.m * v.m;
    unsigned int q1 = round(1.0/6*(v.m-1));   // ex: 1.0/3*4=1
    unsigned int q2 = round(2.0/6*(v.m-1));   // ex: 2.0/3*4=3
    unsigned int q3 = round(4.0/6*(v.m-1));
    unsigned int q4 = round(5.0/6*(v.m-1));
    unsigned int a = 0;
    unsigned int b = 0;
    // First no position is blocked, so everything is zero
    v.blocked_pos = calloc(v.graph->num_vertices, sizeof(unsigned int));
    v.queens[0] = malloc( v.num_queens * sizeof(unsigned int) );
    v.queens[1] = malloc( v.num_queens * sizeof(unsigned int) );
    v.queens_p1[0] = malloc( v.num_queens * sizeof(unsigned int) );
    v.queens_p1[1] = malloc( v.num_queens * sizeof(unsigned int) );
    v.queens_p2[0] = malloc( v.num_queens * sizeof(unsigned int) );
    v.queens_p2[1] = malloc( v.num_queens * sizeof(unsigned int) );
    for (unsigned int i = 0; i < max_pos; ++i) {
        unsigned int row = i / v.m;
        unsigned int col = i % v.m;
        if (row == 0 && (col == q1 || col == q2 || col == q3 || col == q4)) {
            v.queens[0][a] = i;
            v.queens_p1[0][a] = i;
            v.queens_p2[0][a] = i;
            v.blocked_pos[i] = 1;
            ++a;
        }
        else if ((col == 0 || col == v.m-1) && (row == q1 || row == q2)) {
            v.queens[0][a] = i;
            v.queens_p1[0][a] = i;
            v.queens_p2[0][a] = i;
            v.blocked_pos[i] = 1;
            ++a;
        }
        else if (row == v.m-1 && (col == q1 || col == q2 || col == q3 || col == q4)) {;
            v.queens[1][b] = i;
            v.queens_p1[1][b] = i;
            v.queens_p2[1][b] = i;
            ++b;
            v.blocked_pos[i] = 1;
        }
        else if ((col == 0 || col == v.m-1) && (row == q3 || row == q4)) {
            v.queens[1][b] = i;
            v.queens_p1[1][b] = i;
            v.queens_p2[1][b] = i;
            ++b;
            v.blocked_pos[i] = 1;
        }
    }
    for (unsigned int i = 0; i<v.num_queens;++i) {
        printf("P1 Q%d = %d\n",i,v.queens[0][i]);
        printf("P2 Q%d = %d\n",i,v.queens[1][i]);
    }
}

void print_pos() {
    for (unsigned int i = 0; i < v.graph->num_vertices; ++i) {
        if (i % v.m == 0) {
            printf("\n");
        }
        switch (v.blocked_pos[i])
        {
        case 0:
            printf("0 ");
            break;
        case 1:
            printf("1 ");
            break;
        default:
            break;
        }
    }
    printf("\n\n");
}

// Give index queen
unsigned int index_queen(unsigned int pos, unsigned int player) {
    for (unsigned int i = 0; i < v.num_queens; ++i) {
        if (v.queens[player][i] == pos) {
            return i;
        }
    }
    printf("ERROR: Index of Queen couldn't be found!!\n");
    return 55;
}

// Update the current game
void update_positions() {
    // printf("Update positions starts:\n");
    unsigned int queen_old = v.move.queen_src;
    unsigned int index_q = index_queen(queen_old, v.next_player);
    if (v.blocked_pos[queen_old] == 1) {
        v.blocked_pos[queen_old] = 0; 
    }
    else {
        printf("ERROR IN BLOCKED POSITIONS!\n No Queen at the position %d\n", v.move.queen_src);
    }
    if (v.blocked_pos[v.move.queen_dst] == 0) {
        v.blocked_pos[v.move.queen_dst] = 1;
        v.queens[v.next_player][index_q] = v.move.queen_dst;
    }
    else {
        printf("ERROR IN BLOCKED POSITIONS!\n Pos %d is not free for queen move\n", v.move.queen_dst);
    }
    if (v.blocked_pos[v.move.arrow_dst] == 0) {
        v.blocked_pos[v.move.arrow_dst] = 1;
    }
    else {
        printf("ERROR IN BLOCKED POSITIONS!\n Pos %d is not free for shoot arrow\n", v.move.queen_dst);
    }
    // printf("Update positions ended:\n");
}

// Checking if the queen is blocked for server
unsigned int is_queen_blocked_server(unsigned int queen_idx){
    gsl_spmatrix_uint *csr = v.graph->t;
    for (int k = csr->p[queen_idx]; k < csr->p[queen_idx+1]; k++) {
        unsigned int j = csr->i[k];
        if(v.blocked_pos[j] == 0){
            return 0;
        }             
    }
    return 1;
}

// Is the game over?
unsigned int is_game_over() {
    unsigned int p1 = 1;
    unsigned int p2 = 1;
    printf("\n");
    for (unsigned int i = 0; i < v.num_queens; ++i) {
        // printf("P1 is_queen %d blocked? : %d \n",i,is_queen_blocked_server(v.queens[0][i]));
        if (is_queen_blocked_server(v.queens[0][i]) == 0) {
            p1 = 0;
        }
        // printf("P2 is_queen %d blocked? : %d \n",i,is_queen_blocked_server(v.queens[1][i]));
        if (is_queen_blocked_server(v.queens[1][i]) == 0) {
            p2 = 0;
        }
    }
    if (p1 || p2) {
        printf("**************\n");
        printf("Player%d wins!!\n", v.next_player);
        printf("**************\n\n");
        return 1;
    }
    return 0;
}

// Pick a random player who starts first
void start_player() {
    srand(time(NULL));
    v.current_player = rand() % 2;
    v.next_player = 1-v.current_player;
}

// After every round switching players.
void switch_players() {
    v.current_player = 1-v.current_player;
    v.next_player = 1-v.next_player;
    v.previous_move = v.move;
}

// is the move correct?
unsigned int is_move_correct(struct move_t move) {
    if (move.arrow_dst > v.graph->num_vertices || move.queen_dst > v.graph->num_vertices
        || move.queen_src > v.graph->num_vertices) {
        printf("ERROR: Queen or Arrow out of range\n");
        return 0;
    }
    if (v.blocked_pos[move.queen_src] == 0 || v.blocked_pos[move.queen_dst] == 1) {
        printf("ERROR: Unvalid move with queen\n");
        return 0;
    }
    if (move.arrow_dst != move.queen_src && v.blocked_pos[move.arrow_dst] == 1) {
        return 0;
    }
    return 1;
}


void start_game(int argc, char *argv[]) {
    def_game(argc, argv);
    players_information();
    start_player();
    // Initialize the first move.
    v.move.arrow_dst = UINT_MAX;
    v.move.queen_src = UINT_MAX; 
    v.move.queen_dst = UINT_MAX;
    v.previous_move.arrow_dst = UINT_MAX;
    v.previous_move.queen_dst = UINT_MAX;
    v.previous_move.queen_src = UINT_MAX;
    v.graph = generate_graph(v.m, v.t);
    v.graph_p1 = generate_graph(v.m, v.t);
    v.graph_p2 = generate_graph(v.m, v.t);
    if (v.num_queens == 4) {
        initialize_position_queens();
    }
    else {
        initialize_position_queens_eight();
    }
    player1.initialize(v.player1_id, v.graph_p1, v.num_queens, v.queens_p1);
    player2.initialize(v.player2_id, v.graph_p2, v.num_queens, v.queens_p2);
}

// Free everything and close the libs.
void end_function() {
    for(unsigned int i = 0; i<NUM_PLAYERS; ++i) {
        free(v.queens[i]);
    }
    free(v.blocked_pos);
    free_graph(v.graph);
    player1.finalize();
    player2.finalize();
    dlclose(player1.handle);
    dlclose(player2.handle);
    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
    start_game(argc, argv);
    printf("Welcome to the game! The size of game is %d\n", v.graph->num_vertices);
    while(!is_game_over()) {
        print_pos();
        if (v.current_player == player1.player_id) {
            v.move = player1.play(v.previous_move);
            if (is_move_correct(v.move)) {
                printf("P1 Move correct\n");
            }
            else {
                // Incorrect move means the game is over.
                break;
            }
        }
        else {
            v.move = player2.play(v.previous_move);
            if (is_move_correct(v.move)) {
                printf("P2 Move correct\n");
            }
            else {
                break;
            }
        }
        update_positions();
        switch_players();
        // sleep(100);
    }
    end_function();
    return 0;
}