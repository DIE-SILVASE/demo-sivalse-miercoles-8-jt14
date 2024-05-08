/**
 * @file fsm_buzzer.h
 * @brief Header for fsm_buzzer.c file.
 * @author Eduardo García
 * @author Roberto Antolín
 * @date 04/04/2024
 */

#ifndef FSM_BUZZER_H_
#define FSM_BUZZER_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */

#include <stdint.h>
#include <stdbool.h>

/* Other includes */

#include <fsm.h>
#include "melodies.h"

/* HW dependent includes */


/* Defines and enums ----------------------------------------------------------*/
/* Enums */
enum FSM_BUZZER {
  WAIT_START = 0,
  PLAY_NOTE,
  PAUSE_NOTE,
  WAIT_NOTE,
  WAIT_MELODY
};

enum USER_ACTIONS {
  STOP = 0,
  PLAY,
  PAUSE
};

/* Typedefs --------------------------------------------------------------------*/
typedef struct{
    fsm_t f; /*Buzzer melody player FSM*/
    melody_t * p_melody; /*Pointer to the melody to play*/
    uint32_t note_index; /*Index of the current note of the melody to play*/
    uint8_t	buzzer_id; /*Buzzer melody player ID. Must be unique.*/
    uint8_t	user_action; /*Action to perform on the player*/
    double player_speed; /*Speed of the player*/
} fsm_buzzer_t;

/* Function prototypes and explanation -------------------------------------------------*/

/**
* @brief Check if the buzzer finite state machine is playing a melody.
* 
* @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
*/

bool fsm_buzzer_check_activity (fsm_t *p_this);

/**
 * @brief Creates a new buzzer finite state machine.
 * 
 * @param buzzer_id Unique buzzer identifier number.
 * @return fsm_t* 
 */

fsm_t *fsm_buzzer_new(uint32_t buzzer_id);

/**
 * @brief This function initializes the default values of the FSM struct and calls to the port to initialize the HW of associated to the given ID.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @param buzzer_id Unique buzzer identifier number.
 */

void fsm_buzzer_init(fsm_t *p_this, uint32_t buzzer_id);

/**
 * @brief This function sets the melody to play. The user must pass a pointer to the melody to play.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @param p_melody Pointer to the melody to play
 */

void fsm_buzzer_set_melody (fsm_t *p_this, const melody_t *p_melody);

/**
 * @brief This function sets the speed of the player. The user must pass a double value with the speed of the player.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @param speed Speed of the player
 * 
 */

void fsm_buzzer_set_speed (fsm_t *p_this, double speed);

/**
 * @brief This function sets the action to perform on the player. The user must pass a USER_ACTIONS value with the action desired. 
 * These serve as flags to indicate if the user has stopped, paused or started the player, or if the player has stopped itself.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @param action Action to perform on the player.
 * 
 */

void fsm_buzzer_set_action (fsm_t *p_this, uint8_t action);

/**
 * @brief This function returns the action performed on the player. This is useful to know if the player is playing, paused or stopped and a melody has ended.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * 
 */

uint8_t fsm_buzzer_get_action (fsm_t *p_this);
#endif /* FSM_BUZZER_H_ */