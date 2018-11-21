#ifndef MENU_H
#define MENU_H

/* menu prototype */

/**
 * Initializes gameplay environment 
 *
 * @HW: TIMER3
 * @globals: menu_state, wii_init_state, home_state, hs_table_state,
 *           player_select_state, game_loop_state, game_highscore
 */
void menu_init(void);

/**
 * Switches to current gameplay state 
 *
 * @globals: menu_state
 */
void menu_fn(void);

#endif


