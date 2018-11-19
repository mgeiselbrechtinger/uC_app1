#include    <avr/io.h>
#include    <stdint.h>
#include    <string.h>
#include    <stdio.h>

#include    "./mac.h"
#include    "./game_utils.h"
#include    "../rand/rand.h"
#include    "../libwiimote/wii_user.h"
//#include    "../libglcd/glcd.h"
//#include    "../font/Standard5x7.h"
#include    "../glcd/font/Standard5x7.h"
#include    "../glcd/glcd_user/glcd.h"

  //////////////////////
 /* global variables */
//////////////////////

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
    uint8_t accel_x, accel_y, accel_z;
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

static uint16_t game_highscore[5] = {0, 0, 0, 0, 0};

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
/* absolute address of ball */
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

static void game_draw_random_platform(uint8_t rand_platform);

static void game_log_random_platform(uint8_t rand_platform);

static void game_platform_log_update(void);

static void game_draw_ball(xy_point lower_left, void (*drawPx)(const uint8_t, const uint8_t));

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
}

void menu_fn(void)
{
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

static void wii_init_fn(void)
{

    if(wii_init_state == I_INIT){
        error_t status;
        uint8_t i;
        xy_point p = { .x = XSTART_TXT, .y = YSTART_TXT };

        glcdFillScreen(GLCD_CLEAR);
        for(i = 0; i < WII_INIT_TABLE_LEN; i++){
            glcdDrawTextPgm(wii_init_table[i], p, &Standard5x7, &glcdSetPixel);
            p.y += YLINE_TXT;
        }

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
            wii_init_state = I_DISCONNECTED;

        }
    }

}

static void home_fn(void)
{
    if(home_state == I_INIT){

        uint8_t i;
        xy_point p = { .x = XSTART_TXT, .y = YSTART_TXT };

        glcdFillScreen(GLCD_CLEAR);
        /* print title */
        i = 0;
        p.x += 13; 
        glcdDrawTextPgm(menu_table[i], p, &Standard5x7, &glcdSetPixel);
        p.x = XSTART_TXT;
        p.y += 3*YLINE_TXT;
        /* print menu */
        for(i = 1; i < MENU_TABLE_LEN; i++){
            glcdDrawTextPgm(menu_table[i], p, &Standard5x7, &glcdSetPixel);
            p.y += YLINE_TXT;
        }

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

// TODO: cant print 4digit number??
static void hs_table_fn(void)
{
    if(hs_table_state == I_INIT){
        /* 9bytes for "PLAYERx: ", 5bytes for largest number "65535", string terminator */ 
        char txt_buff[15];
        uint8_t i;
        xy_point p = { .x = XSTART_TXT, .y = YSTART_TXT };

        glcdFillScreen(GLCD_CLEAR);
        for(i = 0; i < (HS_TABLE_LEN -1); i++){
            /* append highscore to text */
            memset(txt_buff, 32, 15);
            strcpy_P(txt_buff, (PGM_P)pgm_read_word(&(hs_table[i])));
            sprintf(txt_buff+9, "%u", game_highscore[i]);
            glcdDrawText(txt_buff, p, &Standard5x7, &glcdSetPixel);
            p.y += YLINE_TXT;
        }
        glcdDrawTextPgm(hs_table[i], p, &Standard5x7, &glcdSetPixel);

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

static void player_select_fn(void)
{

    if(player_select_state == I_INIT){
        /* display text */
        char txt_buff[USER_NAME_LEN];
        uint8_t i;
        xy_point p  = { .x = XSTART_TXT, .y = YSTART_TXT };

        glcdFillScreen(GLCD_CLEAR);
        // TODO: can't load directly from PGM
        for(i = 0; i < USER_SELECT_TABLE_LEN; i++){
            strcpy_P(txt_buff, (PGM_P)pgm_read_word(&(user_select_table[i])));
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

static void game_loop_fn(void)
{
    if(game_loop_state == I_INIT){
        /* clear screen */
        glcdFillScreen(GLCD_CLEAR);
        /* initialize score */
        memset(&game_score, 0, sizeof game_score);
		game_score.threshold = 30;
		/* initialize platforms */
        memset(&game_platforms, 0, sizeof game_platforms);
        game_platforms.delay = 13; /* 13 to draw first platfrom imediatly */
		/* initialize ball */
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

        /* manage gameplay */
        game_play();

        /* abort game with home button */
        if(wii_data.button_l == BUTTON_HOME)
            game_loop_state = I_GAME_OVER;

        wii_data.button_l = 0;

    }else if(game_loop_state == I_GAME_OVER){
        /* set highscore entry */
        if(game_score.value > game_highscore[game_player])
            game_highscore[game_player] = game_score.value;

        /* reset Y-shift */
        glcdSetYShift(YEND);

        /* show highscore table */
        game_loop_state = I_INIT;
        menu_state = M_HS_TABLE;
    }	
}

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

static uint8_t  game_choose_random_platform(void)
{
    /* choose random platform */
    return (uint8_t)rand16() & (GAME_PLATFORM_NR - 1); 
}

static void game_draw_random_platform(uint8_t rand_platform)
{
    uint8_t i;
    xy_point p1, p2;

    for(i = 0; i < GAME_PLATFORM_COORDS; i += 2){
        p1.x = game_platform_templates[rand_platform][i];
        p1.y = (game_yshift.value + YSTART) & YMOD;
        p2.x = game_platform_templates[rand_platform][i+1];
        p2.y = (game_yshift.value + YSTART) & YMOD;
        glcdDrawLine(p1, p2, &glcdSetPixel);
    }

}

static void game_log_random_platform(uint8_t rand_platform)
{
    platform_data_t platform_data;
    platform_data.platform_nr = rand_platform;
    platform_data.y_pos = YSTART;

    game_platforms.buff[game_platforms.head] = platform_data;
    game_platforms.head = (game_platforms.head + 1) & 7;
}

static void game_platform_log_update(void)
{
	uint8_t i;

	for(i = game_platforms.tail; i != game_platforms.head; i = (i + 1) & 7){
		/* decrement every platforms y pos */
		game_platforms.buff[i].y_pos = (game_platforms.buff[i].y_pos - 1) & YMOD;

	}
}

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

static void game_collision_check(void)
{
    platform_data_t platform_data;
    uint8_t collision_left, collision_right;
    uint8_t platform_left, platform_right;
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
        
		/* check all windows of platform */
        for(i = 0; i < GAME_PLATFORM_COORDS; i += 2){
            platform_left  = game_platform_templates[platform_data.platform_nr][i];
            platform_right = game_platform_templates[platform_data.platform_nr][i+1];

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

static void game_over_check(void)
{
    /* check if top of ball hit the top of playing field */
    if((game_ball.y - 3) == YEND)
        game_loop_state = I_GAME_OVER;

}

/**
 * Draws ball, TODO maybe use two fill rect functions
 *             ****
 *            ******
 *            ******
 *             ****
 *             ^
 *             |
 *        lower_left
 */
static void game_draw_ball(xy_point lower_left, void (*drawPx)(const uint8_t, const uint8_t))
{
    xy_point right;
    right.x = lower_left.x + 4;
    right.y = lower_left.y;

    //if(lower_left.y <= YEND)
    glcdDrawLine(lower_left, right, drawPx);

    lower_left.x--;
    lower_left.y--;
    right.x++;
    right.y--;

    //if(lower_left.y <= YEND)
    glcdDrawLine(lower_left, right, drawPx);

    lower_left.y--;
    right.y--;

    //if(lower_left.y <= YEND)
    glcdDrawLine(lower_left, right, drawPx);

    lower_left.x++;
    lower_left.y--;
    right.x--;
    right.y--;

    //if(lower_left.y <= YEND)
    glcdDrawLine(lower_left, right, drawPx);

}

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

static void wii_conn_callback(uint8_t wii, connection_status_t status)
{
    /* set connection status */
    wii_data.conn_status = status;

    if(status == DISCONNECTED){
        wii_data.conn_flag = 0;
        menu_state = M_WII_INIT;
    }
}

static void wii_rcv_button(uint8_t wii, uint16_t buttonStates)
{
    wii_data.button_h |= (uint8_t)(buttonStates >> 8);
    wii_data.button_l |= (uint8_t)buttonStates;
}

static void wii_rcv_accel(uint8_t wii, uint16_t x, uint16_t y, uint16_t z)
{
    /* x has 10bit, y and z 9bit precission */
    wii_data.accel_x = (uint8_t)(x >> 2);
    wii_data.accel_y = (uint8_t)(y >> 1);
    wii_data.accel_z = (uint8_t)(z >> 1);
}
