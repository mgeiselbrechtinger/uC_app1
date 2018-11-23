// TODO
// test program once more
// added platforms to progmem

#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdint.h>
#include    <string.h>
#include    <stdio.h>

#include    "../mac.h"
#include    "./game_utils.h"
#include    "../rand/rand.h"
#include    "../libwiimote/wii_user.h"
#include    "./glcd/glcd_user/glcd.h"
#include    "./glcd/font/Standard5x7.h"

//////////////////////
/* global variables */
//////////////////////

uint8_t game_timer_flag;

/* state data */
static M_STATE menu_state;
static I_STATE wii_init_state;
static I_STATE home_state;
static I_STATE hs_table_state;
static I_STATE player_select_state;
static I_STATE game_loop_state;

/* wiimote data */
typedef struct {
    uint8_t conn_flag; 
    connection_status_t conn_status;
    uint8_t button_h, button_l;
    uint8_t accel_x;
} wii_data_t;
static wii_data_t wii_data;

/* player data */
typedef struct{
    xy_point p1, p2;
} player_select_t;
static player_select_t player_select;

static uint8_t game_player;

/* score data */
typedef struct{
    uint8_t tick;
    uint16_t value, threshold;
} game_score_t;
static game_score_t game_score;

typedef struct game_hs_t{
    uint8_t player;
    uint16_t score;
} game_highscore_t;

static game_highscore_t game_highscore[5];

/* yshift data */
typedef struct{
    uint8_t value, flag, tick, threshold;
} game_yshift_t;
static game_yshift_t game_yshift;

/* platform data */
typedef struct {
    uint8_t platform_nr;
    /* absolute address of platform */
    uint8_t y_pos;
}platform_data_t;

typedef struct{
    uint8_t delay;
    uint8_t head, tail;
    platform_data_t buff[8];
} game_platforms_t;
static game_platforms_t game_platforms;

/* ball data */
typedef struct{
    uint8_t left, right;
} game_collision_t;
static game_collision_t game_collision;
/* absolute address of lower left corner of ball, cf. game_draw_ball fn */
static xy_point game_ball;

/////////////////////////
/* internal prototypes */
/////////////////////////

/* menu prototypes */

static void wii_init_fn(void);

static void home_fn(void);

static void hs_table_fn(void);

static void player_select_fn(void);

static void game_loop_fn(void);

/* game prototypes */

static void game_ticks(void);

static void game_play(void);

static uint8_t game_choose_random_platform(void);

static void game_draw_random_platform(const uint8_t rand_platform);

static void game_log_random_platform(const uint8_t rand_platform);

static void game_platform_log_update(void);

static void game_draw_ball(const xy_point lower_left, void (*drawPx)(const uint8_t, const uint8_t));

static void game_set_ball_x(void);

static int8_t game_platform_under_ball(void);

static void game_collision_check(void);

static void game_over_check(void);

/* wii prototypes */

static void wii_conn_callback(uint8_t wii, connection_status_t status);

static void wii_rcv_button(uint8_t wii, uint16_t buttonStates);

static void wii_rcv_accel(uint8_t wii, uint16_t x, uint16_t y, uint16_t z);

//////////////////////////////
/* function implementations */
//////////////////////////////

/**
 * Initializes gameplay environment 
 *
 * @HW: TIMER3
 * @globals: menu_state, wii_init_state, home_state, hs_table_state,
 *           player_select_state, game_loop_state, game_highscore
 */
void menu_init(void)
{
    /* setup TIMER3: 20Hz game ticks */
    TIMSK3 |= (1 << OCIE3A);
    TCNT3 = 0;
    OCR3A = 3125;
    TCCR3A = 0;
    TCCR3B = (1 << WGM32) | (1 << CS32);

    /* initialize states */
    menu_state	        = M_WII_INIT;
    wii_init_state      = I_INIT;
    home_state	        = I_INIT;
    hs_table_state      = I_INIT;
    player_select_state = I_INIT;
    game_loop_state	= I_INIT;

    /* reset highscore table */
    memset(&game_highscore, 0, sizeof game_highscore);
    
    game_timer_flag = 0;
}

/**
 * Switches to current gameplay state 
 *
 * @globals: menu_state
 */
void menu_fn(void)
{
    if(game_timer_flag == 1){
        game_timer_flag = 0;

        switch(menu_state){

            case M_WII_INIT:

                wii_init_fn();
                break;

            case M_HOME:

                home_fn();
                break;

            case M_HS_TABLE:

                hs_table_fn();
                break;

            case M_PLAYER_SELECT:

                player_select_fn();
                break;

            case M_GAME_LOOP:

                game_loop_fn();
                break;

            default:

                break;
        }
    }
}

/**
 * 50ms timer interrupt
 *
 * @globals: game_timer_flag, enables next game tick
 */
ISR(TIMER3_COMPA_vect)
{
    game_timer_flag = 1;
}

/**
 * Handles connection to wiimote
 *
 * @globals: menu_state, wii_init_state, wii_data
 */
static void wii_init_fn(void)
{

    if(wii_init_state == I_INIT){

        error_t status; 
        xy_point p = { .x = XSTART_TXT, .y = YSTART_TXT };

        glcdFillScreen(GLCD_CLEAR);

        glcdDrawTextPgm(wii_init_table[0], p, &Standard5x7, &glcdSetPixel);
        p.y += YLINE_TXT;
        glcdDrawTextPgm(wii_init_table[1], p, &Standard5x7, &glcdSetPixel);

        wii_data.conn_status = 0;
        wii_data.conn_flag = 0;

        status = wiiUserInit(&wii_rcv_button, &wii_rcv_accel);
        if(status == SUCCESS)
            wii_init_state = I_DISCONNECTED;



    }else if(wii_init_state == I_DISCONNECTED){

        /* try connection to wii */
        if(wii_data.conn_flag == 0){
            wiiUserConnect(wii_nr, wii_mac, &wii_conn_callback);
            wii_data.conn_flag = 1;

        }else{
            if(wii_data.conn_status == CONNECTED)
                wii_init_state = I_CONNECTED;	
        }

    }else if(wii_init_state == I_CONNECTED){

        if(wii_data.conn_status == CONNECTED){
            wiiUserSetLeds(wii_nr, wii_nr, 0);
            wii_data.conn_flag = 1;
            menu_state = M_HOME;

        }else{
            wii_init_state = I_INIT;

        }
    }

}

/**
 * Displayes home screen
 *
 * @globals: menu_state, home_state, wii_data
 */
static void home_fn(void)
{
    if(home_state == I_INIT){

        xy_point p = { .x = XSTART_TXT, .y = YSTART_TXT };
        glcdFillScreen(GLCD_CLEAR);

        /* print title */
        p.x += 10; 
        glcdDrawTextPgm(menu_table[0], p, &Standard5x7, &glcdSetPixel);
        p.x = XSTART_TXT;
        p.y += 3*YLINE_TXT;
        /* print menu text */ 
        glcdDrawTextPgm(menu_table[1], p, &Standard5x7, &glcdSetPixel);
        p.y += YLINE_TXT;
        glcdDrawTextPgm(menu_table[2], p, &Standard5x7, &glcdSetPixel);

        home_state = I_IDLE;
        wii_data.button_l = 0;

    }else if(home_state == I_IDLE){

        switch(wii_data.button_l & (BUTTON_1 | BUTTON_2)){

            case BUTTON_1:	home_state = I_INIT;
                                menu_state = M_HS_TABLE;
                                break;

            case BUTTON_2:	home_state = I_INIT;
                                menu_state = M_PLAYER_SELECT;
                                break;

            default:		break;
        }

        wii_data.button_l = 0;
    }
}

/**
 * Displayes highscore table
 *
 * @brief: only entries with more than 0 points are shown
 * @globals: menu_state, hs_table_state, game_highscore, wii_data
 */
static void hs_table_fn(void)
{
    if(hs_table_state == I_INIT){
        char txt_buff[HS_LINE_LEN];
        uint8_t i;
        xy_point p = { .x = XSTART_TXT, .y = YSTART_TXT };

        glcdFillScreen(GLCD_CLEAR);

        /* print highscore only if biggr 0 */
        for(i = 0; i < 5; i++){
            /* append score to name */
            if(game_highscore[i].score > 0){
                memset(txt_buff, HS_ASCII_SPACE, HS_LINE_LEN);
                strncpy_P(txt_buff, (PGM_P)pgm_read_word(&(hs_table[game_highscore[i].player])), HS_NAME_LEN);
                snprintf((txt_buff + HS_NAME_LEN), HS_SCORE_LEN, "%u", game_highscore[i].score);
                glcdDrawText(txt_buff, p, &Standard5x7, &glcdSetPixel);
                p.y += YLINE_TXT;
            }
        }

        p.y = 6*YLINE_TXT;
        /* draw return field */
        glcdDrawTextPgm(hs_table[HS_TABLE_LEN], p, &Standard5x7, &glcdSetPixel);

        /* dissable rumbler after game over */
        wiiUserSetRumbler(wii_nr, 0, 0);

        hs_table_state = I_IDLE;
        wii_data.button_l = 0;

    }else if(hs_table_state == I_IDLE){

        if(wii_data.button_l == BUTTON_B){
            menu_state = M_HOME;
            hs_table_state = I_INIT;
        }

        wii_data.button_l = 0;
    }
}

/**
 * Shows currently selected player
 *
 * @globals: menu_state, player_select_state, game_player, wii_data
 */
static void player_select_fn(void)
{

    if(player_select_state == I_INIT){
        /* display text */
        char txt_buff[USER_NAME_LEN];
        uint8_t i;
        xy_point p  = { .x = XSTART_TXT, .y = YSTART_TXT };

        glcdFillScreen(GLCD_CLEAR);

        /* cannot use glcdDrawTextPgm in loop */   
        for(i = 0; i < USER_SELECT_TABLE_LEN; i++){
            strncpy_P(txt_buff, (PGM_P)pgm_read_word(&(user_select_table[i])), USER_LINE_LEN);
            glcdDrawText(txt_buff, p, &Standard5x7, &glcdSetPixel);
            p.y += YLINE_TXT;
        }

        player_select_state = I_SELECT;

        wii_data.button_h = 0;
        wii_data.button_l = 0;
        game_player = 0;

    }else if(player_select_state == I_SELECT){

        player_select.p1.x = 1;
        player_select.p2.x = USER_NAME_LEN + 1;
        player_select.p2.y = YSTART_TXT + 1 + game_player * YLINE_TXT;
        player_select.p1.y = player_select.p2.y - YLINE_TXT;

        glcdDrawRect(player_select.p1, player_select.p2, &glcdSetPixel);

        player_select_state = I_IDLE;

    }else if(player_select_state == I_IDLE){

        /* check wii arrows */
        switch(wii_data.button_h & (ARROW_UP | ARROW_DOWN)){

            case ARROW_UP:  if(game_player > 0){
                                game_player--;
                                glcdDrawRect(player_select.p1, player_select.p2, &glcdClearPixel);
                                player_select_state = I_SELECT;
                            }
                            break;

            case ARROW_DOWN: if(game_player < 4){
                                 game_player++;
                                 glcdDrawRect(player_select.p1, player_select.p2, &glcdClearPixel);
                                 player_select_state = I_SELECT;
                             }
                             break;

            default:		break;	
        }
        /* check wii buttons */
        switch(wii_data.button_l & (BUTTON_A | BUTTON_B)){

            case BUTTON_A: player_select_state = I_INIT;
                           menu_state = M_GAME_LOOP;
                           break;

            case BUTTON_B: player_select_state = I_INIT;
                           menu_state = M_HOME;
                           break;

            default:	   break;
        }

        wii_data.button_h = 0;
        wii_data.button_l = 0;

    }
}

/**
 * Inherits the gameplay functions
 *
 * @globals: menu_state, game_loop_state, game_score, game_platforms, game_ball,
 *           game_yshift, game_highscore, game_player, game_collision, wii_data
 */
static void game_loop_fn(void)
{
    if(game_loop_state == I_INIT){
        /* clear screen */
        glcdFillScreen(GLCD_CLEAR);
        /* initialize score */
        memset(&game_score, 0, sizeof game_score);
        game_score.threshold = 30;
        /* initialize platforms, delay has to be 13*/
        memset(&game_platforms, 0, sizeof game_platforms);
        game_platforms.delay = 13;
        /* initialize ball */
        memset(&game_collision, 0, sizeof game_collision);
        game_ball.x = XMID;
        game_ball.y = YSTART - 1; 
        /* initialize yshift */
        memset(&game_yshift, 0, sizeof game_yshift);
        game_yshift.flag = 1;
        game_yshift.threshold = 6;
        /* enable accelerometer */
        wiiUserSetAccel(wii_nr, 1, 0);

        game_loop_state = I_PLAY;

    }else if(game_loop_state == I_PLAY){

        game_ticks();

        game_play();

        /* abort game with home button */
        if(wii_data.button_l == BUTTON_HOME)
            game_loop_state = I_GAME_OVER;

        wii_data.button_l = 0;

    }else if(game_loop_state == I_GAME_OVER){
        /* dissable accelerometer */
        wiiUserSetAccel(wii_nr, 0, 0);

        uint8_t i;
        game_highscore_t tmp;

        /* instert score in hs table */
        if(game_score.value > game_highscore[0].score){
            game_highscore[0].player = game_player;
            game_highscore[0].score = game_score.value;

            /* swap till score on right position */
            for(i = 1; i < HS_TABLE_LEN; i++){
                if(game_highscore[i-1].score > game_highscore[i].score){
                    tmp = game_highscore[i];
                    game_highscore[i] = game_highscore[i-1];
                    game_highscore[i-1] = tmp;
                }
            }
        }

        /* reset Y-shift */
        glcdSetYShift(YEND);

        /* show highscore table */
        game_loop_state = I_INIT;
        menu_state = M_HS_TABLE;
    }	
}

/**
 * Counters for different game timings
 *
 * @globals: game_score, game_yshift
 */
static void game_ticks(void)
{
    /* count up seconds */
    game_score.tick++;

    if(game_score.tick == 20){
        game_score.tick = 0;
        /* every second a point */
        game_score.value++;
    }

    /* count up till next shift */
    game_yshift.tick++;

    if(game_yshift.tick == game_yshift.threshold){
        game_yshift.tick = 0;
        game_yshift.flag = 1;
    }

    /* enhance difficulty */
    if(game_score.value == game_score.threshold){
        /* multiply by 2 */
        game_score.threshold <<= 1;

        if(game_yshift.threshold > 0){
            game_yshift.threshold -= 1;
        }
    }
}

/** 
 * Game play update
 *
 * @globals: game_ball, game_yshift, game_platforms
 */
static void game_play(void)
{
    /* remove ball */
    xy_point render_ball;
    render_ball.x = game_ball.x;
    render_ball.y = (game_ball.y + game_yshift.value) & YMOD;
    game_draw_ball(render_ball, &glcdClearPixel);

    if(game_yshift.flag == 1){
        game_yshift.flag = 0;
        game_platforms.delay++;

        /* draw bottom line */
        if(game_platforms.delay == 14){
            game_platforms.delay = 0;
            uint8_t rand_platform = game_choose_random_platform();
            /* draw and log new random platform */
            game_draw_random_platform(rand_platform);
            game_log_random_platform(rand_platform);

        }else{
            /* clear line */
            glcdDrawHorizontal((game_yshift.value + YSTART) & YMOD, &glcdClearPixel);

        }

        /* shift field one up */
        glcdSetYShift(game_yshift.value);

        game_yshift.value = (game_yshift.value + 1) & YMOD;

        /* update absolute y positions */
        game_ball.y = (game_ball.y - 1) & YMOD;
        game_platform_log_update();
    }

    /* set y_coord of ball and restrict x movement */
    game_collision_check();

    /* set x-coord of ball: user input */
    game_set_ball_x();

    /* check for game over */
    game_over_check();

    /* draw ball */
    render_ball.x = game_ball.x;
    render_ball.y = (game_ball.y + game_yshift.value) & YMOD;
    game_draw_ball(render_ball, &glcdSetPixel);

}

/**
 * Chooses random platform 
 *
 * @return: random index to game_platform_templates array
 */ 
static uint8_t game_choose_random_platform(void)
{
    return (uint8_t)rand16() & (GAME_PLATFORM_NR - 1); 
}

/**
 * Draws random platform
 * 
 * @param: index to random game_platform_templates array
 * @globals: game_yshift
 */
static void game_draw_random_platform(const uint8_t rand_platform)
{
    uint8_t i;
    xy_point p1, p2;
    char platform[GAME_PLATFORM_COORDS];

    memcpy_P(&platform, (PGM_P)pgm_read_word(&(game_platform_templates[rand_platform])), sizeof platform);

    for(i = 0; i < GAME_PLATFORM_COORDS; i += 2){
        p1.x = (uint8_t)platform[i];
        p1.y = (game_yshift.value + YSTART) & YMOD;
        p2.x = (uint8_t)platform[i + 1];
        p2.y = (game_yshift.value + YSTART) & YMOD;
        glcdDrawLine(p1, p2, &glcdSetPixel);
    }

}

/**
 * Stores random platform in ringbuffer
 * 
 * @param: index to random game_platform_templates array 
 * @globals: game_platforms
 */
static void game_log_random_platform(const uint8_t rand_platform)
{
    platform_data_t platform_data;
    platform_data.platform_nr = rand_platform;
    platform_data.y_pos = YSTART;

    game_platforms.buff[game_platforms.head] = platform_data;
    game_platforms.head = (game_platforms.head + 1) & 7;
}

/**
 * Updates y coordinate of platforms in ringbuffer 
 *
 * @brief: is needed to stay consistent with ball coordinate
 * @globals: game_platforms
 */
static void game_platform_log_update(void)
{
    uint8_t i;

    /* decrement every platforms y pos */
    for(i = game_platforms.tail; i != game_platforms.head; i = (i + 1) & 7)
        game_platforms.buff[i].y_pos = (game_platforms.buff[i].y_pos - 1) & YMOD;

}

/**
 * Checks if game platform is below or above ball
 *
 * @globals: game_platforms, game_ball
 * @return: -1 if no platform is right under ball
 *          otherwise index to random game_platform_templates array
 */
static int8_t game_platform_under_ball(void)
{
    uint8_t i;
    int8_t distance;

    for(i = game_platforms.tail; i != game_platforms.head; i = (i + 1) & 7){
        /* calculate distance between ball and platform */
        distance = game_ball.y - game_platforms.buff[i].y_pos;

        /* platform above ball, remove from buffer */
        if(distance > 5){
            game_platforms.tail = (game_platforms.tail + 1) & 7;

            /* platform right below or besides ball, return */
        }else if(distance >= -1 && distance <= 3){
            return +i;

        }
    }

    /* no platform under ball */
    return -1;
}

/**
 * Checks if ball hits platform
 *
 * @globals: game_platforms, game_collision, game_ball
 */
static void game_collision_check(void)
{
    platform_data_t platform_data;
    uint8_t collision_left, collision_right;
    uint8_t platform_left, platform_right;
    char platform[GAME_PLATFORM_COORDS];
    uint8_t ball_left, ball_right;
    uint8_t gravity, i;
    int8_t platform_idx;

    ball_left = game_ball.x - 1;
    ball_right = game_ball.x + 5;

    collision_left = 0;
    collision_right = 0;

    /* set gravity */
    gravity = 1;

    platform_idx = game_platform_under_ball();

    /* platform under ball */
    if(platform_idx != -1){
        platform_data = game_platforms.buff[platform_idx];
        memcpy_P(&platform, (PGM_P)pgm_read_word(&(game_platform_templates[platform_data.platform_nr])), sizeof platform);
        /* check all windows of platform */
        for(i = 0; i < GAME_PLATFORM_COORDS; i += 2){
            platform_left = (uint8_t)platform[i];
            platform_right = (uint8_t)platform[i + 1];

            /* ball not over window, unset gravity */
            if(ball_right >= platform_left && ball_left <= platform_right){
                gravity = 0;
            }
            /* check collision with platform */
            if(ball_right == (platform_left - 1))
                collision_right = 1;

            if(ball_left == (platform_right + 1))
                collision_left = 1;

        }
    }

    /* collision corner cases */
    if(ball_left == 0)
        collision_left = 1;

    if(ball_right == 127)
        collision_right = 1;

    game_collision.left = collision_left;
    game_collision.right = collision_right;

    /* let ball fall */
    if(gravity == 1 && game_ball.y < (YSTART - 1))
        game_ball.y = (game_ball.y + 1) & YMOD;

}

/** 
 * Checks if ball hit top of screen
 *
 * @globals: game_ball, game_loop_state
 */
static void game_over_check(void)
{
    /* check if top of ball hit the top of playing field */
    if((game_ball.y - 3) == YEND){
        wiiUserSetRumbler(wii_nr, 1, 0);
        game_loop_state = I_GAME_OVER;
    }

}

/**
 * Draws ball in the following shape:
 *             ****
 *            ******
 *            ******
 *             ****
 *             ^
 *             |
 *        lower_left
 *
 *  @param: lower_left, holds the coordinates of the lower left corner of the ball
 *  @param drawPx, pointer to pixel manipulation function
 */
static void game_draw_ball(const xy_point lower_left, void (*drawPx)(const uint8_t, const uint8_t))
{
    xy_point left, right;
    left.x = lower_left.x;
    left.y = lower_left.y;
    right.x = lower_left.x + 4;
    right.y = lower_left.y;

    //if(lower_left.y <= YEND)
    glcdDrawLine(left, right, drawPx);

    left.x--;
    left.y--;
    right.x++;
    right.y--;

    //if(lower_left.y <= YEND)
    glcdDrawLine(left, right, drawPx);

    left.y--;
    right.y--;

    //if(lower_left.y <= YEND)
    glcdDrawLine(left, right, drawPx);

    left.x++;
    left.y--;
    right.x--;
    right.y--;

    //if(lower_left.y <= YEND)
    glcdDrawLine(left, right, drawPx);
}

/**
 * Updates the x coordinate of ball according to wii accelerometers
 *
 * @globals: game_collision, game_ball, wii_data
 */
static void game_set_ball_x(void)
{
    /* wii tillted left */
    if(wii_data.accel_x < TILT_LEFT){
        if(game_collision.left == 0)
            game_ball.x--;
    }
    /* wii tillted right */
    if(wii_data.accel_x > TILT_RIGHT){
        if(game_collision.right == 0)
            game_ball.x++;
    }
}

/**
 * Callback function for connection status of wiimote
 *
 * @param: wii, index of wiimote
 * @param: status, holds connection status update
 * @globals: wii_data, menu_state
 */
static void wii_conn_callback(uint8_t wii, connection_status_t status)
{
    (void)wii;

    /* set connection status */
    wii_data.conn_status = status;

    if(status == DISCONNECTED){
        wii_data.conn_flag = 0;
        menu_state = M_WII_INIT;
    }
}

/**
 * Callback function to retrieve wiimote button states
 *
 * @param: wii, index of wiimote
 * @param: buttonStates, holds current button states
 * @globals: wii_data;
 */
static void wii_rcv_button(uint8_t wii, uint16_t buttonStates)
{
    (void)wii;

    wii_data.button_h |= (uint8_t)(buttonStates >> 8);
    wii_data.button_l |= (uint8_t)buttonStates;
}

/** 
 * Callback function to retrieve wiimote accelerometer data
 *
 * @param wii, index of wiimote
 * @param x, y, z, data of accelerometer x, y, z respective
 * @globals: wii_data
 */
static void wii_rcv_accel(uint8_t wii, uint16_t x, uint16_t y, uint16_t z)
{
    (void)wii;
    (void)y;
    (void)z;

    /* x has 10bit, y and z 9bit precission */
    wii_data.accel_x = (uint8_t)(x >> 2);
}
