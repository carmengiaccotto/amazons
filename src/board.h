#ifndef _AMAZON_BOARD_H_
#define _AMAZON_BOARD_H_

// Creating an enum for diffrent types a position can have
enum pos_type {
    EMPTY = 0,
    QUEEN = 1,
    QUEEN_OPP = 2,
    BLOCKED = 3,
};

// Creating a struct with an array of max position the board have
typedef struct board_s {
    enum pos_type *pos;
} board_t;

#endif
