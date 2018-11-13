#include    <avr/io.h>
#include    <stdint.h>
#include    <string.h>
#include    <stdio.h>

#include    "./menu.h"
#include    "./text.h"
#include	"../rand/rand.h"
#include    "../libwiimote/wii_user.h"
#include    "../libglcd/glcd.h"
#include    "../font/Standard5x7.h"

static M_STATE menu_state	    	= M_WII_INIT;
static I_STATE wii_init_state	    = I_INIT;
static I_STATE home_state	    	= I_INIT;
static I_STATE hs_table_state	    = I_INIT;
static I_STATE player_select_state  = I_INIT;
static I_STATE game_loop_state	    = I_INIT;

/* wii_init_fn globals */
static const uint8_t wii_nr = 1;
static const uint8_t wii_mac[6] = { 0x58, 0xbd, 0xa3, 0x4b, 0xf6, 0x80 };
static uint8_t wii_button_h, wii_button_l;
static uint8_t wii_accel_x, wii_accel_y, wii_accel_z;
static connection_status_t wii_conn_status;
static uint8_t wii_conn_flag;

/* player select globals */
static xy_point player_select_p1, player_select_p2;

/* gameplay globals */
static uint8_t game_sec_tick;
static uint16_t game_score;
static uint8_t game_player;
static uint16_t game_highscore[5] = {0, 0, 0, 0, 0};
static uint8_t game_platform_delay;
static uint8_t game_yshift;
static xy_point game_ball;
static uint8_t game_curr_platforms_head;
static uint8_t game_curr_platforms_tail;
static uint8_t game_curr_platforms_len;
typedef struct game_platform_data_t{
	uint8_t platform_nr;
	uint8_t y_pos;
}game_platform_data;
static game_platform_data game_curr_platforms[8];
static uint8_t game_block_x;
static uint8_t game_collision_left, game_collision_right;

void menu_fn(void)
{
    switch(menu_state){

        case M_WII_INIT:

            wii_init_fn(&menu_state);
            break;

        case M_HOME:

            home_fn(&menu_state);
            break;

        case M_HS_TABLE:

            hs_table_fn(&menu_state);
            break;

        case M_PLAYER_SELECT:

            player_select_fn(&menu_state);
            break;

        case M_GAME_LOOP:

            game_loop_fn(&menu_state);
            break;

        default:

            break;
    }
}

void wii_init_fn(M_STATE *m_state)
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

        wii_conn_status = 0;
        wii_conn_flag = 0;

        status = wiiUserInit(&wii_rcv_button, &wii_rcv_accel);
        if(status == SUCCESS)
            wii_init_state = I_DISCONNECTED;



    }else if(wii_init_state == I_DISCONNECTED){

        /* try connection to wii */
        error_t status;

        if(wii_conn_flag == 0){
            status = wiiUserConnect(wii_nr, wii_mac, &wii_conn_callback);
            //if(status == SUCCESS)
            wii_conn_flag = 1;

        }else{
            if(wii_conn_status == CONNECTED)
                wii_init_state = I_CONNECTED;	
        }

    }else if(wii_init_state == I_CONNECTED){

        if(wii_conn_status == CONNECTED){
            wiiUserSetLeds(wii_nr, wii_nr, 0);
            wii_conn_flag = 1;
            (*m_state) = M_HOME;

        }else{
            wii_init_state = I_DISCONNECTED;

        }
    }

}

void home_fn(M_STATE *m_state)
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
        wii_button_l = 0;

    }else if(home_state == I_IDLE){

        switch(wii_button_l & (BUTTON_1 | BUTTON_2)){

            case BUTTON_1:	home_state = I_INIT;
                                (*m_state) = M_HS_TABLE;
                                break;

            case BUTTON_2:	home_state = I_INIT;
                                (*m_state) = M_PLAYER_SELECT;
                                break;

            default:		break;
        }

        wii_button_l = 0;
    }
}

// TODO: cant print 4digit number??
void hs_table_fn(M_STATE *m_state)
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
        wii_button_l = 0;

    }else if(hs_table_state == I_IDLE){

        if(wii_button_l == BUTTON_B){
            (*m_state) = M_HOME;
            hs_table_state = I_INIT;
        }

        wii_button_l = 0;
    }
}

void player_select_fn(M_STATE *m_state)
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
        wii_button_h = 0;
        wii_button_l = 0;
        game_player = 0;

    }else if(player_select_state == I_SELECT){

        player_select_p1.x = 1;
        player_select_p2.x = USER_NAME_LEN + 1;
        player_select_p2.y = YSTART_TXT + 1 + game_player * YLINE_TXT;
        player_select_p1.y = player_select_p2.y - YLINE_TXT;

        glcdDrawRect(player_select_p1, player_select_p2, &glcdSetPixel);

        player_select_state = I_IDLE;

    }else if(player_select_state == I_IDLE){

        /* check wii arrows */
        switch(wii_button_h & (ARROW_UP | ARROW_DOWN)){

            case ARROW_UP:  if(game_player > 0){
                                game_player--;
                                glcdDrawRect(player_select_p1, player_select_p2, &glcdClearPixel);
                                player_select_state = I_SELECT;
                            }
                            break;

            case ARROW_DOWN: if(game_player < 4){
                                 game_player++;
                                 glcdDrawRect(player_select_p1, player_select_p2, &glcdClearPixel);
                                 player_select_state = I_SELECT;
                             }
                             break;

            default:		break;	
        }
        /* check wii buttons */
        switch(wii_button_l & (BUTTON_A | BUTTON_B)){

            case BUTTON_A: player_select_state = I_INIT;
                           (*m_state) = M_GAME_LOOP;
                           break;

            case BUTTON_B: player_select_state = I_INIT;
                           (*m_state) = M_HOME;
                           break;

            default:	   break;
        }

        wii_button_h = 0;
        wii_button_l = 0;

    }
}

void game_loop_fn(M_STATE *m_state)
{
    if(game_loop_state == I_INIT){
        /* clear screen */
        glcdFillScreen(GLCD_CLEAR);

        /* reset game variables */
        game_platform_delay = 13; /* 13 to draw first platfrom imediatly */
		game_curr_platforms_head = 0;
		game_curr_platforms_tail = 0;
		game_curr_platforms_len = 0;
        game_sec_tick = 0;
        game_score = 0;
        game_ball.x = XMID;
        game_ball.y = YSTART - 30; // TODO: change back to -1
        game_yshift = glcdGetYShift();

        game_loop_state = I_PLAY;

    }else if(game_loop_state == I_PLAY){
        /* count up seconds and score */
        game_sec_tick++;
        if(game_sec_tick == 19){
            game_sec_tick = 0;
            game_score++;
        }

        /* manage gameplay */
        game_play();
        
        /* abort game with home button */
        if(wii_button_l == BUTTON_HOME)
        	game_loop_state = I_GAME_OVER;

    	wii_button_l = 0;
	
	}else if(game_loop_state == I_GAME_OVER){
        /* set highscore entry */
        if(game_score > game_highscore[game_player])
            game_highscore[game_player] = game_score;
	
		/* reset Y-shift */
		glcdSetYShift(YEND);

        /* show highscore table */
        game_loop_state = I_INIT;
        (*m_state) = M_HS_TABLE;
    }	
}

void game_play(void)
{
    /* remove ball */
    game_draw_ball(game_ball, &glcdClearPixel);
    
	/* every second shift game field */
   	if(game_sec_tick == 1){
   	    game_platform_delay++;
   		
   		/* draw bottom line */
   		if(game_platform_delay == 14){
   			game_platform_delay = 0;
			uint8_t rand_platform = game_choose_random_platform();
			/* draw and log new random platform */
   			game_draw_random_platform(rand_platform);
   			game_log_random_platform(rand_platform);
   		
		}else{
   			glcdDrawHorizontal(game_yshift + YSTART, &glcdClearPixel);
   		
		}
   		
   	    /* shift field one up */
   	    glcdSetYShift(game_yshift);
   		
   		game_yshift++;
   	}
	
    /* set y_coord of ball */
	game_collision_check();
	
	/* set x-coord of ball: user input */
	game_set_ball_x();
	
    /* check for game over */
	game_over_check();
	
    /* draw ball */
    game_draw_ball(game_ball, &glcdSetPixel);

}

uint8_t  game_choose_random_platform(void)
{
    /* choose random platform */
    return (uint8_t)rand16() & (GAME_PLATFORM_NR - 1); 
}

void game_draw_random_platform(uint8_t rand_platform)
{
    uint8_t i;
    xy_point p1, p2;

    for(i = 0; i < GAME_PLATFORM_COORDS; i += 2){
        p1.x = game_platforms[rand_platform][i];
        p1.y = game_yshift + YSTART;
        p2.x = game_platforms[rand_platform][i+1];
        p2.y = game_yshift + YSTART;
        glcdDrawLine(p1, p2, &glcdSetPixel);
    }

}

void game_log_random_platform(uint8_t rand_platform)
{
	game_platform_data platform_data;
	platform_data.platform_nr = rand_platform;
	platform_data.y_pos = game_yshift + YSTART;
	
	game_curr_platforms[game_curr_platforms_head] = platform_data;
	game_curr_platforms_head = (game_curr_platforms_head + 1) & 7;
	game_curr_platforms_len++;
}

int8_t game_platform_under_ball(void)
{
	uint8_t i;
	int8_t distance;
	
	for(i = game_curr_platforms_tail; i != game_curr_platforms_head; i = (i + 1) & 7){
		/* calculate distance between ball and platform */
		distance = game_ball.y - game_curr_platforms[i].y_pos - 1;

		/* platform above ball, remove from buffer */
		if(distance < -4){
			game_curr_platforms_tail = (game_curr_platforms_tail + 1) & 7;
			game_curr_platforms_len--;
		
		/* platform underneath or besides ball, return */
		}else if(distance >= -4 && distance <= 0){
			return +i;
		
		}
	}

	/* no platform under ball */
	return -1;
}

void game_collision_check(void)
{
	game_platform_data platform_data;
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
		platform_data = game_curr_platforms[platform_idx];

		/* check all windows of platform */
		for(i = 0; i < GAME_PLATFORM_COORDS; i += 2){
			platform_left  = game_platforms[platform_data.platform_nr][i];
			platform_right = game_platforms[platform_data.platform_nr][i+1];
			
			/* ball not over window, unset gravity */
			if(ball_right >= platform_left && ball_left <= platform_right)
				gravity = 0;
			
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

	game_collision_left = collision_left;
	game_collision_right = collision_right;

	/* let ball fall */
	if(gravity == 1 && game_ball.y < (YSTART + game_yshift - 1))
		game_ball.y++;
		
}

void game_over_check(void)
{
	/* check if ball hit the top */
	if((game_ball.y - 3) == (game_yshift + YEND))
		game_loop_state = I_GAME_OVER;

}

/**
 * Draws ball  ****
 *            ******
 *            ******
 *             ****
 *             ^
 *             |
 *        lower_left
 */
void game_draw_ball(xy_point lower_left, void (*drawPx)(const uint8_t, const uint8_t))
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

// TODO: use accelerometer x (0 = 128, left = pos, right = neg)
void game_set_ball_x(void)
{
   	/* check wii arrows */
   	switch(wii_button_h & (ARROW_LEFT | ARROW_RIGHT)){

   	    case ARROW_LEFT: if(game_collision_left == 0)
   	                    	game_ball.x--;
   	                     break;

   	    case ARROW_RIGHT: if(game_collision_right == 0)
   						  	game_ball.x++;
   	                      break;

   	    default:		break;	
   	}
	
	wii_button_h = 0;
}

void wii_conn_callback(uint8_t wii, connection_status_t status)
{
    /* set connection status */
    wii_conn_status = status;

    if(status == DISCONNECTED){
        wii_conn_flag = 0;
        menu_state = M_WII_INIT;
    }
}

void wii_rcv_button(uint8_t wii, uint16_t buttonStates)
{
    wii_button_h |= (uint8_t)(buttonStates >> 8);
    wii_button_l |= (uint8_t)buttonStates;
}

void wii_rcv_accel(uint8_t wii, uint16_t x, uint16_t y, uint16_t z)
{
	/* x has 10bit, y and z 9bit precission */
    wii_accel_x = (uint8_t)(x >> 2);
    wii_accel_y = (uint8_t)(y >> 1);
    wii_accel_z = (uint8_t)(z >> 1);
}
