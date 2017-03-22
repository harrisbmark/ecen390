/*********************************************************************************/
/* File: minimax.c                                                               */
/* Purpose: To provide the functions necessary to run an artificially            */
/*          intelligent game of tic-tac-toe using the minimax algorithm.         */
/* Author: Mark Harris                                                           */
/* Date: 10-25-16                                                                */
/*********************************************************************************/
#include "minimax.h"
#include <stdio.h>
#include <string.h>

// Necessary defines to specify the height and width of the board (used to print the tic-tac-toe board).
#define MINIMAX_BOARD_WIDTH 11
#define MINIMAX_BOARD_HEIGHT 5

// Necessary defines to specify a modulus for the horizontal and vertical lines used to print the tic-tac-toe board.
#define MINIMAX_BOARD_HORIZONTAL_MODULUS 2
#define MINIMAX_BOARD_VERTICAL_MODULUS 4

#define MINIMAX_BOARD_STARTING_DEPTH 0
#define MINIMAX_BOARD_CURRENT_SQUARE 0
#define MINIMAX_BOARD_NEXT_SQUARE 1

// Create a static global variable to contain the choice of minimax.
static minimax_move_t choice;

/*********************************************************************************/
/* Function: minimax_is_valid                                                    */
/* Purpose: To check if the value passed is a player or opponent value.          */
/* Returns: A boolean value.                                                     */
/*********************************************************************************/
bool minimax_is_valid(uint8_t value)
{
    // If the value passed is a player or an opponent value, return true.
    if (value == MINIMAX_PLAYER_SQUARE || value == MINIMAX_OPPONENT_SQUARE)
    {
        return true;
    }
    
    // Otherwise, return false.
    else
    {
        return false;
    }
}

/*********************************************************************************/
/* Function: minimax_print_board                                                 */
/* Purpose: To print a tic-tac-toe board with the current values marked.         */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void minimax_print_board(minimax_board_t* board)
{
    // Create variables used to specify the location of the row and column on the board.
    uint8_t board_x = MINIMAX_BOARD_CURRENT_SQUARE;
    uint8_t board_y = MINIMAX_BOARD_CURRENT_SQUARE;
    
    // For each row in the tic-tac-toe board.
    for (uint8_t y = MINIMAX_BOARD_CURRENT_SQUARE; y < MINIMAX_BOARD_HEIGHT; y++)
    {   
        // For each column in the tic-tac-toe board.
        for (uint8_t x = MINIMAX_BOARD_CURRENT_SQUARE; x < MINIMAX_BOARD_WIDTH; x++)
        {
            // If we haven't hit the modulus value for the tic-tac-toe row, print a vertical bar ("|").
            if (!((x + MINIMAX_BOARD_NEXT_SQUARE) % MINIMAX_BOARD_VERTICAL_MODULUS))
            {
                printf("|");
            }

            // Otherwise, if we haven't hit the modulus value for the tic-tac-toe column, print a dash ("-").
            else if (!((y + MINIMAX_BOARD_NEXT_SQUARE) % MINIMAX_BOARD_HORIZONTAL_MODULUS))
            {
                printf("-");
            }

            // Otherwise.
            else
            {   
                // If we haven't hit the modulus value for the previous column.
                if (!((x - MINIMAX_BOARD_NEXT_SQUARE) % MINIMAX_BOARD_VERTICAL_MODULUS))
                {
                    // If the value on the board is a valid character, put the character at that position on the board.
                    if (minimax_is_valid(board->squares[board_y][board_x]))
                    {
                        printf("%c", board->squares[board_y][board_x]);
                    }
                    
                    // Otherwise, print a blank space on the board.
                    else
                    {
                        printf(" ");
                    }
                    
                    // If we haven't hit the modulus value for the previous row, then increment y.
                    if (!((y - MINIMAX_BOARD_NEXT_SQUARE) % MINIMAX_BOARD_HORIZONTAL_MODULUS))
                    {
                        board_y++;
                    }
                    
                    // Increment x.
                    board_x++;
                }
                
                // Otherwise, print a blank space.
                else
                {
                    printf(" ");
                }
            }
        }
        
        // Print a newline to make the output look nice.
        printf("\n\r");
    }
}

/*********************************************************************************/
/* Function: minimax_board_character_count                                       */
/* Purpose: To return how many characters (X's and O's) are occupying the board. */
/* Returns: An int value.                                                        */
/*********************************************************************************/
uint8_t minimax_board_character_count(minimax_board_t* board)
{
    // Create a temp variable to store the character count.
    uint8_t squares_size = MINIMAX_BOARD_CURRENT_SQUARE;
    
    // For every row in the tic-tac-toe board.
    for (uint8_t y = MINIMAX_BOARD_CURRENT_SQUARE; y < MINIMAX_BOARD_COLUMNS; y++)
    {
        // For every column in the tic-tac-toe board.
        for (uint8_t x = MINIMAX_BOARD_CURRENT_SQUARE; x < MINIMAX_BOARD_ROWS; x++)
        {
            // If the character on the board is valid, then increment the character count.
            if (minimax_is_valid(board->squares[y][x]))
            {
                squares_size++;
            }
        }
    }
    
    // Return the character count (the "squares" size).
    return squares_size;
}

/*********************************************************************************/
/* Function: minimax_win_horizontal                                              */
/* Purpose: To return whether or not a win occurred on a horizontal.             */
/* Returns: A boolean value.                                                     */
/*********************************************************************************/
bool minimax_win_horizontal(minimax_board_t* board)
{
    // For every row in the board.
    for (uint8_t y = MINIMAX_BOARD_CURRENT_SQUARE; y < MINIMAX_BOARD_ROWS; y++)
    {
        // For every column in the board where the last column is equal to the current column.
        for (uint8_t x = MINIMAX_BOARD_NEXT_SQUARE; x < MINIMAX_BOARD_COLUMNS && minimax_is_valid(board->squares[y][x]) && minimax_is_valid(board->squares[y][x - MINIMAX_BOARD_NEXT_SQUARE]) && board->squares[y][x] == board->squares[y][x - MINIMAX_BOARD_NEXT_SQUARE]; x++)
        {            
            // If we get to the last column, then we know a win has occurred and can return true.
            if (x == MINIMAX_BOARD_COLUMNS - MINIMAX_BOARD_NEXT_SQUARE)
            {
                return true;
            }
        }
    }
    
    // Return false if we get to here (no wins were found).
    return false;
}

/*********************************************************************************/
/* Function: minimax_win_vertical                                                */
/* Purpose: To return whether or not a win occurred on a vertical.               */
/* Returns: A boolean value.                                                     */
/*********************************************************************************/
bool minimax_win_vertical(minimax_board_t* board)
{
    // For every column in the board.
    for (uint8_t x = MINIMAX_BOARD_CURRENT_SQUARE; x < MINIMAX_BOARD_COLUMNS; x++)
    {
        // For every row in the board where the last row is equal ot the current row.
        for (uint8_t y = MINIMAX_BOARD_NEXT_SQUARE; y < MINIMAX_BOARD_ROWS && minimax_is_valid(board->squares[y][x]) && minimax_is_valid(board->squares[y - MINIMAX_BOARD_NEXT_SQUARE][x]) && board->squares[y][x] == board->squares[y - MINIMAX_BOARD_NEXT_SQUARE][x]; y++)
        {
            // If we get to the last row, then we know a win has occurred and can return true.
            if (y == MINIMAX_BOARD_ROWS - MINIMAX_BOARD_NEXT_SQUARE)
            {
                return true;
            }
        }
    }
    
    // Return false if we get to here (no wins were found).
    return false;
}

/*********************************************************************************/
/* Function: minimax_win_diagonal                                                */
/* Purpose: To return whether or not a win occurred on a diagonal.               */
/* Returns: A boolean value.                                                     */
/*********************************************************************************/
bool minimax_win_diagonal(minimax_board_t* board)
{
    // For every column and row where the last left diagonal is equal to the current diagonal.
    for (uint8_t x = MINIMAX_BOARD_NEXT_SQUARE, y = MINIMAX_BOARD_NEXT_SQUARE; x < MINIMAX_BOARD_COLUMNS && y < MINIMAX_BOARD_ROWS && minimax_is_valid(board->squares[y][x]) && minimax_is_valid(board->squares[y - MINIMAX_BOARD_NEXT_SQUARE][x - MINIMAX_BOARD_NEXT_SQUARE]) && board->squares[y][x] == board->squares[y - MINIMAX_BOARD_NEXT_SQUARE][x - MINIMAX_BOARD_NEXT_SQUARE]; x++, y++)
    {
        // If we get to the bottom right corner of the board, then we know a win has occurred and can return true.
        if (x == MINIMAX_BOARD_COLUMNS - MINIMAX_BOARD_NEXT_SQUARE && y == MINIMAX_BOARD_ROWS - MINIMAX_BOARD_NEXT_SQUARE)
        {
            return true;
        }
    }
    
    // For every column and row here the last right diagonal is equal to the current diagonal.
    for (uint8_t x = MINIMAX_BOARD_COLUMNS - MINIMAX_BOARD_NEXT_SQUARE, y = MINIMAX_BOARD_CURRENT_SQUARE; x >= MINIMAX_BOARD_CURRENT_SQUARE && y < MINIMAX_BOARD_ROWS && minimax_is_valid(board->squares[y][x]) && minimax_is_valid(board->squares[y + MINIMAX_BOARD_NEXT_SQUARE][x - MINIMAX_BOARD_NEXT_SQUARE]) && board->squares[y][x] == board->squares[y + MINIMAX_BOARD_NEXT_SQUARE][x - MINIMAX_BOARD_NEXT_SQUARE]; x--, y++)
    {
        // If we get to the middle of the board, then we know a win has occurred and can return true.
        if (x - MINIMAX_BOARD_NEXT_SQUARE == MINIMAX_BOARD_CURRENT_SQUARE && y + MINIMAX_BOARD_NEXT_SQUARE == MINIMAX_BOARD_ROWS - MINIMAX_BOARD_NEXT_SQUARE)
        {
            return true;
        }
    }
        
    // Return false if we get to here (no wins were found)).
    return false;
}

/*********************************************************************************/
/* Function: minimax_highest_score                                               */
/* Purpose: To return the location of the highest score in the score table.      */
/* Returns: A minimax_move_t value.                                              */
/*********************************************************************************/
minimax_move_t minmax_highest_score(minimax_score_t score_table[MINIMAX_BOARD_COLUMNS][MINIMAX_BOARD_ROWS])
{
    // Define and initialize temporary variables to control which location has the highest score.
    minimax_move_t return_move;
    uint8_t max_y = MINIMAX_BOARD_CURRENT_SQUARE;
    uint8_t max_x = MINIMAX_BOARD_CURRENT_SQUARE;
    int16_t max_value = MINIMAX_OPPONENT_WINNING_SCORE;
    
    // For every row in the tic-tac-toe board.
    for (uint8_t y = MINIMAX_BOARD_CURRENT_SQUARE; y < MINIMAX_BOARD_ROWS; y++)
    {
        // For every column in the tic-tac-toe board.
        for (uint8_t x = MINIMAX_BOARD_CURRENT_SQUARE; x < MINIMAX_BOARD_COLUMNS; x++)
        {
            // If the current location has a score higher than the current temporary max value.
            if (score_table[y][x] != MINIMAX_USED_SQUARE && score_table[y][x] >= max_value)
            {
                // Set the max value to the current score. Set the temporary max x and max y variables to that location.
                max_value = score_table[y][x];
                max_y = y;
                max_x = x;
            }
        }
    }
    
    // Now set the return move variable coordinates to the max x and max y.
    return_move.column = max_x;
    return_move.row = max_y;
    
    // Return the location of the highest score.
    return return_move;
}

/*********************************************************************************/
/* Function: minimax_lowest_score                                                */
/* Purpose: To return the location of the lowest score in the score table.       */
/* Returns: A minimax_move_t value.                                              */
/*********************************************************************************/
minimax_move_t minmax_lowest_score(minimax_score_t score_table[MINIMAX_BOARD_COLUMNS][MINIMAX_BOARD_ROWS])
{
    // Define and initialize temporary variables to control which location has the highest score.
    minimax_move_t return_move;
    uint8_t min_y = MINIMAX_BOARD_CURRENT_SQUARE;
    uint8_t min_x = MINIMAX_BOARD_CURRENT_SQUARE;
    int16_t min_value = MINIMAX_PLAYER_WINNING_SCORE;
    
    // For every row in the tic-tac-toe board.
    for (uint8_t y = MINIMAX_BOARD_CURRENT_SQUARE; y < MINIMAX_BOARD_ROWS; y++)
    {
        // For every column in the tic-tac-toe board.
        for (uint8_t x = MINIMAX_BOARD_CURRENT_SQUARE; x < MINIMAX_BOARD_COLUMNS; x++)
        {
            // If the current location has a score less than or equal to the current temporary min value.
            if (score_table[y][x] != MINIMAX_USED_SQUARE && score_table[y][x] <= min_value)
            {
                // Set the min value to the current score. Set the temporary min x and min y variables to that location.
                min_value = score_table[y][x];
                min_y = y;
                min_x = x;
            }
        }
    }
    
    // Now set the return move variable coordinates to the min x and min y.
    return_move.column = min_x;
    return_move.row = min_y;
    
    // Return the location of the lowest score.
    return return_move;
}

/*********************************************************************************/
/* Function: minimax                                                             */
/* Purpose: To return a score base on recursion through the tic-tac-toe table    */
/*          finding the optimal spot to place a mark.                            */
/* Returns: A minimax_score_t value.                                             */
/*********************************************************************************/
minimax_score_t minimax(minimax_board_t* board, bool player, uint8_t depth)
{   
    // Used for debugging purposes to show the depth and current layout of the board.
    //printf("Invoking minimax @ depth = %d\n\r", depth);
    //minimax_print_board(board);

    // Get the score of the current board and place it into a variable called current score.
    minimax_score_t current_score = minimax_computeBoardScore(board, player);
    
    // If the game is over (based on the score), then return the current score.
    if (minimax_isGameOver(current_score))
    {
        return current_score;
    }
    
    // Otherwise.
    else
    {
        // Increase the depth and create a score table.
        depth++;
        minimax_score_t score_table[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS];
        
        // For every row in the board.
        for (uint8_t y = MINIMAX_BOARD_CURRENT_SQUARE; y < MINIMAX_BOARD_ROWS; y++)
        {
            // For every column in the board.
            for (uint8_t x = MINIMAX_BOARD_CURRENT_SQUARE; x < MINIMAX_BOARD_COLUMNS; x++)
            {
                // If the location on the board is an empty square.
                if (board->squares[y][x] == MINIMAX_EMPTY_SQUARE)
                {
                    // If player is true (meaning we are currently an X).
                    if (player)
                    {
                        // Used for debugging purposes to show where we are adding a mark.
                        // printf("Adding player @ (%d, %d)\n\n\r", y, x);

                        // Set the current location to be a player mark.
                        board->squares[y][x] = MINIMAX_PLAYER_SQUARE;
                    }
                    
                    // Otherwise (we are currently an O).
                    else
                    {
                        // Used for debugging purposes to show where we are adding a mark.
                        // printf("Adding opponent @ (%d, %d)\n\n\r", y, x);

                        // Set the current location to be an opponent mark.
                        board->squares[y][x] = MINIMAX_OPPONENT_SQUARE;
                    }
                    
                    // Call minimax recursively and place the score into the score table, then set the current location to an empty square.
                    score_table[y][x] = minimax(board, !player, depth);
                    board->squares[y][x] = MINIMAX_EMPTY_SQUARE;
                }

                // Otherwise, the location is "used" and we will mark it that way.
                else
                {
                    score_table[y][x] = MINIMAX_USED_SQUARE;
                }
           }
        }
        
        // If player (meaning that we are currently an X).
        if (player)
        {
            // Set the choice to the highest possible score in the score table and set the current score to that score.
            choice = minmax_highest_score(score_table);
            current_score = score_table[choice.row][choice.column];
        }
        
        // Otherwise (we are currently an O).
        else
        {
            // Set the choice to the lowest possible score in the score table and set the current score to that score.
            choice = minmax_lowest_score(score_table);
            current_score = score_table[choice.row][choice.column];
        }
    }
    
    // Return the current score.
    return current_score;
}

/*********************************************************************************/
/* Function: minimax_computeNextMove                                             */
/* Purpose: To compute the next move to take for the tic-tac-toe game (AI).      */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void minimax_computeNextMove(minimax_board_t* board, bool player, uint8_t* row, uint8_t* column)
{
    // Call minimax with the passed board, passed player, and starting depth value.
    minimax_score_t score = minimax(board, player, MINIMAX_BOARD_STARTING_DEPTH);
    
    // Set the row and column to the global choice and column.
    *row = choice.row;
    *column = choice.column;

    // Used for debugging purposes to show the final score of the computed move.
    // printf("Final score: %d\nFinal choice: (row: %d, column: %d)\n\r", score, choice.row, choice.column);
}

/*********************************************************************************/
/* Function: minimax_isGameOver                                                  */
/* Purpose: To return whether or not the game is over based on the score.        */
/* Returns: A boolean value.                                                     */
/*********************************************************************************/
bool minimax_isGameOver(minimax_score_t score)
{
    // If the score is equal to a winning score for the player or opponent or a draw score, return true.
    if (score == MINIMAX_PLAYER_WINNING_SCORE || score == MINIMAX_OPPONENT_WINNING_SCORE || score == MINIMAX_DRAW_SCORE)
    {
        return true;
    }
    
    // Otherwise, return false.
    else
    {
        return false;
    }
}

/*********************************************************************************/
/* Function: minimax_isGameOver                                                  */
/* Purpose: To return whether or not the game is over based on the board layout. */
/* Returns: A boolean value.                                                     */
/*********************************************************************************/
bool minimax_isGameOver(minimax_board_t* board)
{
    // If there is a win diagonally, horizontally, or vertically, then return true.
    if (minimax_win_diagonal(board) || minimax_win_horizontal(board) || minimax_win_vertical(board))
    {
        return true;
    }

    // Otherwise, if the board is maxed out (meaning all spaces are filled), then return true.
    else if (minimax_board_character_count(board) == MINIMAX_BOARD_ROWS * MINIMAX_BOARD_COLUMNS)
    {
        return true;
    }

    // Otherwise, return false.
    else
    {
        return false;
    }
}

/*********************************************************************************/
/* Function: minimax_computeBoardScore                                           */
/* Purpose: To return the score of the board.                                    */
/* Returns: A minimax_score_t value.                                             */
/*********************************************************************************/
minimax_score_t minimax_computeBoardScore(minimax_board_t* board, bool player)
{    
    // If we are not the player and there is a win.
    if (!player && (minimax_win_diagonal(board) || minimax_win_horizontal(board) || minimax_win_vertical(board)))
    {
        // Used for debugging purposes to show that the player wins and the score.
        //printf("Player wins, returning score: %d\n\r", MINIMAX_PLAYER_WINNING_SCORE);

        // Return the player winning score.
        return MINIMAX_PLAYER_WINNING_SCORE;
    }
    
    // Otherwise, if we are the player and there is a win.
    else if (player && (minimax_win_diagonal(board) || minimax_win_horizontal(board) || minimax_win_vertical(board)))
    {
        // Used for debugging purposes to show that the player wins and the score.
        // printf("Opponent wins, returning score: %d\n\r",MINIMAX_OPPONENT_WINNING_SCORE);

        // Return the opponent winning score.
        return MINIMAX_OPPONENT_WINNING_SCORE;
    }
    
    // Otherwise, if the board is not full, return that it is not the end of the game.
    else if (minimax_board_character_count(board) != MINIMAX_BOARD_ROWS * MINIMAX_BOARD_COLUMNS)
    {
        return MINIMAX_NOT_ENDGAME;
    }
    
    // Otherwise.
    else
    {
        // Used for debugging purposes to show that there was a tie.
        // printf("Draw, returning score: %d\n\r", MINIMAX_DRAW_SCORE);

        // Return that a tie was drawn.
        return MINIMAX_DRAW_SCORE;
    }
}

/*********************************************************************************/
/* Function: minimax_initBoard                                                   */
/* Purpose: To initialize the board to empty squares.                            */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void minimax_initBoard(minimax_board_t* board)
{
    // For every row in the board.
    for (uint8_t y = MINIMAX_BOARD_CURRENT_SQUARE; y < MINIMAX_BOARD_ROWS; y++)
    {
        // For every column in the board.
        for (uint8_t x = MINIMAX_BOARD_CURRENT_SQUARE; x < MINIMAX_BOARD_COLUMNS; x++)
        {
            // Set the board location to an empty square.
            board->squares[y][x] = MINIMAX_EMPTY_SQUARE;
        }
    }
}
