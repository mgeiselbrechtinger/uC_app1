#ifndef MENU_H
#define MENU_H

#include    <stdint.h>
#include    "../libglcd/glcd.h"
#include    "../libwiimote/wii_user.h"

/* menu states */
typedef enum{
    M_WII_INIT,
    M_HOME,
    M_HS_TABLE,
    M_PLAYER_SELECT,
    M_GAME_LOOP,
} M_STATE;

/* intern states */
typedef enum{
    I_INIT,
    I_DISCONNECTED,
    I_CONNECTED,
    I_SELECT,
    I_PLAY,
    I_GAME_OVER,
    I_IDLE,
} I_STATE;


/* menu prototypes */
void menu_fn(void);

void wii_init_fn(M_STATE *m_state);

void home_fn(M_STATE *m_state);

void hs_table_fn(M_STATE *m_state);

void player_select_fn(M_STATE *m_state);

void game_loop_fn(M_STATE *m_state);

void game_play(void);

uint8_t game_choose_random_platform(void);

void game_draw_random_platform(uint8_t random_platform);

void game_draw_ball(xy_point lower_left, void (*drawPx)(const uint8_t, const uint8_t));

/* wii constants */
/* first (high) byte of button */
#define ARROW_DOWN	(0x04)
#define	ARROW_UP	(0x08)
#define ARROW_LEFT	(0x01)
#define	ARROW_RIGHT	(0x02)
#define	BUTTON_PLUS	(0x10)
/* second (low) byte of button */
#define	BUTTON_1	(0x02)
#define BUTTON_2	(0x01)
#define	BUTTON_A	(0x08)
#define	BUTTON_B	(0x04)
#define	BUTTON_MINUS (0x10)
#define BUTTON_HOME	(0x80)

/* wii prototypes */
void wii_conn_callback(uint8_t wii, connection_status_t status);

void wii_rcv_button(uint8_t wii, uint16_t buttonStates);

void wii_rcv_accel(uint8_t wii, uint16_t x, uint16_t y, uint16_t z);

#endif


