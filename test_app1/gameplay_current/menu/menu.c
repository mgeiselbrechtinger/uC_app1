#include	<avr/io.h>
#include	<stdint.h>
#include	<string.h>
#include	<stdio.h>

#include	"./menu.h"
#include	"./text.h"
#include	"../libwiimote/wii_user.h"
#include    "../libglcd/glcd.h"
#include    "../font/Standard5x7.h"

static M_STATE menu_state			= M_WII_INIT;
static I_STATE wii_init_state		= I_INIT;
static I_STATE home_state			= I_INIT;
static I_STATE hs_table_state		= I_INIT;
static I_STATE player_select_state	= I_INIT;
static I_STATE game_loop_state		= I_INIT;

/* wii_init_fn globals */
static const uint8_t wii_nr = 1;
static const uint8_t wii_mac[6] = { 0x58, 0xbd, 0xa3, 0xba, 0xa1, 0x32 };
static uint8_t wii_button_h, wii_button_l;
static uint16_t wii_accel_x, wii_accel_y, wii_accel_z;
static connection_status_t wii_conn_status;
static uint8_t wii_conn_flag;

/* player select globals */
static xy_point player_select_p1, player_select_p2;

/* gameplay globals */
static volatile uint8_t game_sec_tick;
static volatile uint16_t game_score;
static volatile uint8_t game_player;
static volatile uint16_t game_highscore[] = {0, 0, 0, 0, 0};

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
   		xy_point p = { .x = XSTART, .y = YSTART };

		glcdFillScreen(GLCD_CLEAR);
		for(i = 0; i < WII_INIT_TABLE_LEN; i++){
			glcdDrawTextPgm(wii_init_table[i], p, &Standard5x7, &glcdSetPixel);
			p.y += YLINE;
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
		xy_point p = { .x = XSTART, .y = YSTART };
    	
		glcdFillScreen(GLCD_CLEAR);
		/* print title */
		i = 0;
		p.x += 13; 
		glcdDrawTextPgm(menu_table[i], p, &Standard5x7, &glcdSetPixel);
		p.x = XSTART;
		p.y += 3*YLINE;
		/* print menu */
		for(i = 1; i < MENU_TABLE_LEN; i++){
    		glcdDrawTextPgm(menu_table[i], p, &Standard5x7, &glcdSetPixel);
			p.y += YLINE;
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

void hs_table_fn(M_STATE *m_state)
{
	if(hs_table_state == I_INIT){

    	char txt_buff[15];
    	char int_buff[6];
    	uint8_t i;
    	xy_point p = { .x = XSTART, .y = YSTART };

    	glcdFillScreen(GLCD_CLEAR);
    	for(i = 0; i < (HS_TABLE_LEN -1); i++){
			// append higscore to text
    	    sprintf(int_buff, "%d", game_highscore[i]);
    	    strcpy_P(txt_buff, (PGM_P)pgm_read_word(&(hs_table[i])));
    	    strcat(txt_buff, int_buff);
    	    glcdDrawText(txt_buff, p, &Standard5x7, &glcdSetPixel);
    	    p.y += YLINE;
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

// TODO: implement user select
void player_select_fn(M_STATE *m_state)
{

	if(player_select_state == I_INIT){
		/* display text */
		char txt_buff[USER_NAME_LEN];
    	uint8_t i;
    	xy_point p  = { .x = XSTART, .y = YSTART };

    	glcdFillScreen(GLCD_CLEAR);
		for(i = 0; i < USER_SELECT_TABLE_LEN; i++){
    	    strcpy_P(txt_buff, (PGM_P)pgm_read_word(&(user_select_table[i])));
    		glcdDrawText(txt_buff, p, &Standard5x7, &glcdSetPixel);
			p.y += YLINE;
		}
	
		player_select_state = I_SELECT;
		wii_button_h = 0;
		wii_button_l = 0;
		game_player = 0;

	}else if(player_select_state == I_SELECT){
		
		player_select_p1.x = 1;
		player_select_p2.x = USER_NAME_LEN + 1;
		player_select_p2.y = YSTART + 1 + game_player * YLINE;
		player_select_p1.y = player_select_p2.y - YLINE;
		
		glcdDrawRect(player_select_p1, player_select_p2, &glcdSetPixel);
		
		player_select_state = I_IDLE;

	}else if(player_select_state == I_IDLE){
		
		/* check wii arrows */
		switch(wii_button_h & (ARROW_UP | ARROW_DOWN)){
			
			case ARROW_UP:	if(game_player > 0){
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
		
			case BUTTON_A:	hs_table_state = I_INIT;
							(*m_state) = M_GAME_LOOP;
							break;

			case BUTTON_B: hs_table_state = I_INIT;
							(*m_state) = M_HOME;
							break;

			default:		break;
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
		game_sec_tick = 0;
		game_score = 0;

		// TODO: Init game play
		
		game_loop_state = I_PLAY;

	}else if(game_loop_state == I_PLAY){
		/* count up seconds and score */
		game_sec_tick++;
		if(game_sec_tick == 19){
			game_sec_tick = 0;
			game_score++;
		}

		// TODO: call gameplay function
		if(game_score == 5)
			game_loop_state = I_GAME_OVER;

	}else if(game_loop_state == I_GAME_OVER){
		/* set highscore entry */
		if(game_score > game_highscore[game_player])
			game_highscore[game_player] = game_score;

		/* show highscore table */
		game_loop_state = I_INIT;
		(*m_state) = M_HS_TABLE;
	}	
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
	wii_accel_x = x;
	wii_accel_y = y;
	wii_accel_z = z;
}
