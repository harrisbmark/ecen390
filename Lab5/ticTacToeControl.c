/*********************************************************************************/
/* File: ticTacToeControl.c                                                      */
/* Purpose: To provide the functions necessary to run a tic-tac-toe game on the  */
/*          Zybo board.                                                          */
/* Author: Mark Harris                                                           */
/* Date: 10-25-16                                                                */
/*********************************************************************************/
#include "ticTacToeControl.h"
#include "ticTacToeDisplay.h"
#include "minimax.h"
#include "../lab2/buttons.h"
#include "supportFiles/display.h"
#include <stdbool.h>
#include <stdio.h>

// Necessary defines to control the timing of the state machine.
#define TIC_TAC_TOE_CONTROL_START_TIME_COUNTER_MAX 40
#define TIC_TAC_TOE_CONTROL_ADC_COUNTER_MAX 1
#define TIC_TAC_TOE_CONTROL_INTRODUCTION_TIME_COUNTER_MAX 100

// Necessary defines to control the clear values for the timers in the state machine.
#define TIC_TAC_TOE_CONTROL_START_TIME_COUNTER_CLEAR 0
#define TIC_TAC_TOE_CONTROL_ADC_COUNTER_CLEAR 0
#define TIC_TAC_TOE_CONTROL_INTRODUCTION_TIME_COUNTER_CLEAR 0

#define TIC_TAC_TOE_CONTROL_HEIGHT_HALF DISPLAY_HEIGHT / 3
#define TIC_TAC_TOE_CONTROL_ABSOLUTE_LEFT 0
#define TIC_TAC_TOE_CONTROL_TEXT_SIZE 2
#define TIC_TAC_TOE_CONTROL_TEXT_PADDING 15

#define TIC_TAC_TOE_CONTROL_TEXT_COLOR DISPLAY_GREEN
#define TIC_TAC_TOE_CONTROL_BACKGROUND_COLOR DISPLAY_BLACK
#define TIC_TAC_TOE_BOARD_DEFAULT_SQUARE 0

// Necessary define to specify the sequence of buttons for a game reset.
#define TIC_TAC_TOE_CONTROL_RESET_SEQUENCE 0b0001

// Volatile variable used to declare when the tick function should run.
volatile unsigned char timer_flag = 0;

// Enum used to create the sates for the tic-tac-toe game.
enum ticTacToeControl_st_t
{
    ticTacToeControl_start_st,                      // The starting state for just one tick.
    ticTacToeControl_intro_init_st,                 // The state for initializing the variables for the intro.
    ticTacToeControl_intro_st,                      // The state for showing the intro to the user.
    ticTacToeControl_intro_uninit_st,               // The state where we erase the intro.
    ticTacToeControl_init_st,                       // The initialization state used to define variables.
    ticTacToeControl_game_start_st,                 // The state where the player chooses to go first or second (to be X or 0).
    ticTacToeControl_minimax_st,                    // The state where minimax is ran to get the best move.
    ticTacToeControl_mark_computer_st,              // The state where the computer choice is marked.
    ticTacToeControl_game_over_st,                  // The state where the game is over and we wait for btn0 to be pressed.
    ticTacToeControl_wait_for_touch_st,             // The state where we wait for the user to decide where to go.
    ticTacToeControl_wait_adc_st,                   // The state where we wait for the ADC to settle.
    ticTacToeControl_is_already_marked_player_st    // The state where we check if the player has chosen a spot that is already marked.
} current_state = ticTacToeControl_start_st;

// Global variables used for the timers in the state machine.
static uint32_t start_time;
static uint32_t adc_counter;
static uint32_t introduction_timer;

// Global variables used to define the player (who is X and who is O), whether or not we are in the first move, the game board, and the row and column of the game board we are on.
static bool player;
static bool first_move;
static minimax_board_t* board;
static uint8_t row, column;

void ticTacToeControl_print_intro(bool erase)
{
    uint16_t color = TIC_TAC_TOE_CONTROL_TEXT_COLOR;

    if (erase)
    {
        color = TIC_TAC_TOE_CONTROL_BACKGROUND_COLOR;
    }

    display_setTextColor(color);
    display_setTextSize(TIC_TAC_TOE_CONTROL_TEXT_SIZE);
    display_setCursor(TIC_TAC_TOE_CONTROL_ABSOLUTE_LEFT + TIC_TAC_TOE_CONTROL_TEXT_PADDING, TIC_TAC_TOE_CONTROL_HEIGHT_HALF);
    display_println("Touch board to play X");
    display_println("          -or-");
    display_println("   wait for the computer");
    display_println("       and play O.");
}

/*********************************************************************************/
/* Function: ticTacToeControl_debug_print                                        */
/* Purpose: To output the current state we are in for the tic-tac-toe control    */
/*          state machine. Used for debugging.                                   */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void ticTacToeControl_debug_print()
{
    // Create static variables storing the previous state and if we are on the first pass.
    static ticTacToeControl_st_t previous_state;
    static bool firstPass = true;

    // If the previous state is different from the current state or if we are on the first pass.
    if (previous_state != current_state || firstPass)
    {
        // Set the first pass equal to false. Set the previous state equal to the current state.
        firstPass = false;
        previous_state = current_state;

        // Switch on the current state to show what output to deliver.
        switch(current_state)
        {
            // If we are in the start state, then output that result to the user.
            case ticTacToeControl_start_st:
                printf("start_st\n\r");
                break;

            // Otherwise, if we are in the intro init state, then output that result to the user.
            case ticTacToeControl_intro_init_st:
                printf("intro_init_st\n\r");
                break;

            // Otherwise, if we are in the intro state, then output that result to the user.
            case ticTacToeControl_intro_st:
                printf("intro_st\n\r");
                break;

            // Otherwise, if we are in the intro uninit state, then output that result to the user.
            case ticTacToeControl_intro_uninit_st:
                printf("intro_uninit_st\n\r");

            // Otherwise, if we are in the init state, then output that result to the user.
            case ticTacToeControl_init_st:
                printf("init_st\n\r");
                break;

            // Otherwise, if we are in the game start state, then output that result to the user.
            case ticTacToeControl_game_start_st:
                printf("game_start_st\n\r");
                break;

            // Otherwise, if we are in the minimax state, then output that result to the user.
            case ticTacToeControl_minimax_st:
                printf("minimax_st\n\r");
                break;

            // Otherwise, if we are in the mark computer state, then output that result to the user.
            case ticTacToeControl_mark_computer_st:
                printf("mark_computer_st\n\r");
                break;

            // Otherwise, if we are in the game over state, then output that result to the user.
            case ticTacToeControl_game_over_st:
                printf("game_over_st\n\r");
                break;

            // Otherwise, if we are in the wait for touch state, then output that result to the user.
            case ticTacToeControl_wait_for_touch_st:
                printf("wait_for_touch_st\n\r");
                break;

            // Otherwise, if we are in the wait adc state, then output that result to the user.
            case ticTacToeControl_wait_adc_st:
                printf("wait_adc_st\n\r");
                break;

            // Otherwise, if we are in the already marked plahyer state, then output that result to the user.
            case ticTacToeControl_is_already_marked_player_st:
                printf("is_already_marked_st\n\r");
                break;
     }
  }
}

/*********************************************************************************/
/* Function: ticTacToeControl_tick                                               */
/* Purpose: To run a "tick" for the tic-tac-toe state machine. This runs through */
/*          the states necessary to play the game.                               */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void ticTacToeControl_tick()
{
    // Switch on the current state. This switch statement is for the state machine transitions and mealy outputs.
    switch (current_state)
    {
        // If we are in the start state, then jump immediately to the next state.
        case ticTacToeControl_start_st:
            current_state = ticTacToeControl_intro_init_st;
            break;

        // Otherwise, if we are in the intro init state, then jump immediately to the intro state.
        case ticTacToeControl_intro_init_st:
            current_state = ticTacToeControl_intro_st;
            break;

        // Otherwise, if we are in the intro state.
        case ticTacToeControl_intro_st:
            // If the introduction timer is past the desired value, then jump to the init state.
            if (introduction_timer >= TIC_TAC_TOE_CONTROL_INTRODUCTION_TIME_COUNTER_MAX)
            {
                current_state = ticTacToeControl_intro_uninit_st;
            }

            // Otherwise, stay in the intro state.
            else
            {
                current_state = ticTacToeControl_intro_st;
            }

            break;

        // Otherwise, if we are in the intro uninit state, then jump to the init sate.
        case ticTacToeControl_intro_uninit_st:
            current_state = ticTacToeControl_init_st;
            break;

        // Otherwise, if we are in the init state, then jump immediately to the game start state.
        case ticTacToeControl_init_st:
            display_clearOldTouchData();
            current_state = ticTacToeControl_game_start_st;
            break;

        // Otherwise, if we are in the game start state.
        case ticTacToeControl_game_start_st:
            // If the start time timer is past the desired value.
            if (start_time >= TIC_TAC_TOE_CONTROL_START_TIME_COUNTER_MAX)
            {
                // Set the player to true (meaning the player is an O), set the first move to true, and then set the current state to the minimax sate.
                player = true;
                first_move = true;
                current_state = ticTacToeControl_minimax_st;
            }

            // Otherwise, if the display is touched.
            else if (display_isTouched())
            {
                // Set the player to false (meaning the player is an X) and set the current state to wait adc.
                player = false;
                current_state = ticTacToeControl_wait_adc_st;
            }

            // Otherwise, stay in the game start state.
            else
            {
                current_state = ticTacToeControl_game_start_st;
            }

            break;

        // Otherwise, if we are in the minimax state, then jump immediately to the mark computer state.
        case ticTacToeControl_minimax_st:
            current_state = ticTacToeControl_mark_computer_st;
            break;

        // Otherwise, if we are in the mark computer state.
        case ticTacToeControl_mark_computer_st:
            // If the game is over, then go to the game over state.
            if (minimax_isGameOver(board))
            {
                current_state = ticTacToeControl_game_over_st;
            }

            // Otherwise, go to the wait for touch state (waiting for the players choice).
            else
            {
                current_state = ticTacToeControl_wait_for_touch_st;
            }

            break;

        // Otherwise, if we are in the game over state.
        case ticTacToeControl_game_over_st:
            // If we get the reset sequence back from the buttons, then go to the init state.
            if ((buttons_read() & TIC_TAC_TOE_CONTROL_RESET_SEQUENCE) == TIC_TAC_TOE_CONTROL_RESET_SEQUENCE)
            {
                current_state = ticTacToeControl_init_st;
            }

            // Otherwise, stay in the game over state.
            else
            {
                current_state = ticTacToeControl_game_over_st;
            }

            break;

        // Otherwise, if we are in the wait for touch state.
        case ticTacToeControl_wait_for_touch_st:
            // If the display is touched.
            if (display_isTouched())
            {
                // Set the current state to the wait adc state and clear the old touch data.
                display_clearOldTouchData();
                current_state = ticTacToeControl_wait_adc_st;
            }

            // Otherwise, stay in the wait for touch state.
            else
            {
                current_state = ticTacToeControl_wait_for_touch_st;
            }

            break;

        // Otherwise, if we are in the wait adc state.
        case ticTacToeControl_wait_adc_st:
            // If the adc counter is past the set time.
            if (adc_counter >= TIC_TAC_TOE_CONTROL_ADC_COUNTER_MAX)
            {
                // Set the current state to the is already marked player state and get the coordinates of the touch point on the touch screen.
                ticTacToeDisplay_touchScreenComputeBoardRowColumn(&row, &column);
                current_state = ticTacToeControl_is_already_marked_player_st;
            }

            // Otherwise, stay in the wait adc state.
            else
            {
                current_state = ticTacToeControl_wait_adc_st;
            }

            break;

        // Otherwise, if we are in the is already marked player state.
        case ticTacToeControl_is_already_marked_player_st:
            // If the current square given by the touch point is empty.
            if (board->squares[row][column] == MINIMAX_EMPTY_SQUARE)
            {
                // If the game is over, then set the current state to game over.
                if (minimax_isGameOver(board))
                {
                    current_state = ticTacToeControl_game_over_st;
                }

                // Otherwise.
                else
                {
                    // If player (meaning the player is an O).
                    if (player)
                    {
                        // Mark the board with the opponent value and draw an O on the screen.
                        board->squares[row][column] = MINIMAX_OPPONENT_SQUARE;
                        ticTacToeDisplay_drawO(row, column, false);
                    }

                    // Otherwise, mark the board with the computer value and draw an X on the screen.
                    else
                    {
                        board->squares[row][column] = MINIMAX_PLAYER_SQUARE;
                        ticTacToeDisplay_drawX(row, column, false);
                    }

                    // Set the current state to the minimax state.
                    current_state = ticTacToeControl_minimax_st;
                }
            }

            // Otherwise, set the current state to the wait for touch state.
            else
            {
                current_state = ticTacToeControl_wait_for_touch_st;
            }

            break;

        // Otherwise, if all else fails, go to the start state.
        default:
            current_state = ticTacToeControl_start_st;
            break;
    }

    // Switch on the current state. This switch is used for computations completed in the state (moore outputs).
    switch (current_state)
    {
        // If we are in the start sate, then don't do anything.
        case ticTacToeControl_start_st:
            break;

        // Otherwise, if we are in the intro init state.
        case ticTacToeControl_intro_init_st:
            introduction_timer = TIC_TAC_TOE_CONTROL_INTRODUCTION_TIME_COUNTER_CLEAR;
            break;

        // Otherwise, if we are in the intro state.
        case ticTacToeControl_intro_st:
            introduction_timer++;
            ticTacToeControl_print_intro(false);
            break;

        // Otherwise, if we are in the intro uninit state, clear the intro message.
        case ticTacToeControl_intro_uninit_st:
            ticTacToeControl_print_intro(true);
            break;

        // Otherwise, if we are in the init state.
        case ticTacToeControl_init_st:
            // Set the global variables used to their "reset" values.
            start_time = TIC_TAC_TOE_CONTROL_START_TIME_COUNTER_CLEAR;
            adc_counter = TIC_TAC_TOE_CONTROL_ADC_COUNTER_CLEAR;
            player = true;
            first_move = false;

            // Reset the board for the display and the internal program.
            ticTacToeDisplay_reset();
            ticTacToeDisplay_drawBoardLines();
            minimax_initBoard(board);
            break;

        // Otherwise, if we are in the game start state, then increment the start time counter variable.
        case ticTacToeControl_game_start_st:
            start_time++;
            break;

        // Otherwise, if we are in the minimax state.
        case ticTacToeControl_minimax_st:
            // If it is the first move.
            if (first_move)
            {
                // Hard-code the first row and column (saving time for computation). Then set first move to false.
                row = column = TIC_TAC_TOE_BOARD_DEFAULT_SQUARE;
                first_move = false;
            }

            // Otherwise.
            else
            {
                // Compute the next move for the computer player.
                minimax_computeNextMove(board, player, &row, &column);
            }

            break;

        // Otherwise, if we are in the mark computer state.
        case ticTacToeControl_mark_computer_st:
            // If player (meaning that the player is an O).
            if (player)
            {
                // Mark the board with a player value and draw an X on the screen.
                board->squares[row][column] = MINIMAX_PLAYER_SQUARE;
                ticTacToeDisplay_drawX(row, column, false);
            }

            // Otherwise.
            else
            {
                // Mark the board with an opponent value and draw an O on the screen.
                board->squares[row][column] = MINIMAX_OPPONENT_SQUARE;
                ticTacToeDisplay_drawO(row, column, false);
            }

            break;

        // Otherwise, if we are in the game over state, don't do anything.
        case ticTacToeControl_game_over_st:
            break;

        // Otherwise, if we are in the wait for touch state, then reset the adc timer variable.
        case ticTacToeControl_wait_for_touch_st:
            adc_counter = TIC_TAC_TOE_CONTROL_ADC_COUNTER_CLEAR;
            break;

            // Otherwise, if we are in the wait adc state, then increment the adc timer variable.
        case ticTacToeControl_wait_adc_st:
            adc_counter++;
            break;

        // Otherwise, if we are in the is already marked player state, then don't do anything.
        case ticTacToeControl_is_already_marked_player_st:
            break;

        // Otherwise, for the default case, don't do anything.
        default:
            break;
    }

    // Used for debugging purposes to show what state we are in.
    // ticTacToeControl_debug_print();
}
