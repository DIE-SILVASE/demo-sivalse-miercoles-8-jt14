/**
 * @file fsm_buzzer.c
 * @brief Buzzer melody player FSM main file.
 * @author Eduardo García
 * @author Roberto Antolín
 * @date 04/04/2024
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C libraries */

#include <stdlib.h>
#include <math.h>

/* Other libraries */

#include "port_buzzer.h"
#include "fsm_buzzer.h"
#include "melodies.h"

/* State machine input or transition functions */

/**
 * @brief This function is the interface between the FSM and the HW. It sets the PWM frequency and the timer duration.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @param freq Frequency of the note to play.
 * @param duration Duration of the note to play.
 */

static void _start_note (fsm_t *p_this, double freq, uint32_t duration){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    uint32_t note_duration = (uint32_t) round(((double) duration) / p_fsm->player_speed);
    port_buzzer_set_note_duration (p_fsm->buzzer_id, note_duration);
    port_buzzer_set_note_frequency (p_fsm->buzzer_id, freq);
}

/**
 * @brief Check a melody is set to start.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @return true
 * @return false
 * 
 */

static bool check_melody_start (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    return (p_fsm->p_melody != NULL && p_fsm->user_action == PLAY);
}

/**
 * @brief Check if the player is set to start.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @return true
 * @return false
 */

static bool check_player_start (fsm_t *p_this){
    return check_melody_start(p_this);
}

/**
 * @brief Check if it has been reached the end of the melody.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @return true
 * @return false
 */

static bool check_end_melody (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    return (p_fsm->note_index >= p_fsm->p_melody->melody_length);
}

/**
 * @brief Check if the player is set to pause.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @return true
 * @return false
 */

static bool check_pause (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    return (p_fsm->user_action == PAUSE);
}

/**
 * @brief Check if the player is set to play.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @return true
 * @return false
 */

static bool check_resume (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    return (p_fsm->user_action == PLAY);
}

/**
 * @brief Check if the player is set to stop.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @return true
 * @return false
 */

static bool check_player_stop (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    return (p_fsm->user_action == STOP);
}

/**
 * @brief Check if the player is set to play a note. If so, later the FSM will update the note and the duration.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @return true 
 * @return false 
 */

static bool check_play_note (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    return(p_fsm->note_index <= p_fsm->p_melody->melody_length && p_fsm->user_action == PLAY);
}

/**
 * @brief Check if the note has ended.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @return true
 * @return false
 */

static bool check_note_end (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    return port_buzzer_get_note_timeout(p_fsm->buzzer_id);
}

/**
 * @brief This function starts a melody player by setting the PWM frequency and the timer duration of the first note. This function is called when the player is set to play only.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * 
 */

static void do_melody_start (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    double freq = p_fsm->p_melody->p_notes[0];
    uint16_t duration = p_fsm->p_melody->p_durations[0];
    _start_note(p_this, freq, duration);
    p_fsm->note_index++;
}

/**
 * @brief This function starts the player by starting a melody.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * 
 */

static void do_player_start (fsm_t *p_this){
    do_melody_start(p_this);
}

/**
 * @brief This function stops the player by stopping the PWM and the timer. This function is called when the player is set to stop.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * 
 */

static void do_end_melody (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    port_buzzer_stop(p_fsm->buzzer_id);
    p_fsm->note_index = 0;
    p_fsm->user_action = STOP;
}

/**
 * @brief This function pauses the player by stopping the PWM and the timer. This function is called when the player is set to pause.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * 
 */

static void do_pause (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    port_buzzer_stop(p_fsm->buzzer_id);
}

/**
 * @brief This function stops the player by stopping the PWM and the timer. This function is called when the player is set to stop.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * 
 */

static void do_player_stop (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    port_buzzer_stop(p_fsm->buzzer_id);
    p_fsm->note_index = 0;
}

/**
 * @brief This function updates the player retrieving the frequency and the duration of the next note of the melody. This function is called when the player is set to play a new note.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * 
 */

static void do_play_note (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    double freq = p_fsm->p_melody->p_notes[p_fsm->note_index];
    uint16_t duration = p_fsm->p_melody->p_durations[p_fsm->note_index];
    _start_note(p_this, freq, duration);
    p_fsm->note_index++;
}

/**
 * @brief This function ends the note by stopping the PWM and the timer. This function is called when the note has ended.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * 
 */

static void do_note_end (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    port_buzzer_stop(p_fsm->buzzer_id);
}

/* State machine output or action functions */

/**
 * @brief Tabla de transiciones de la FSM del buzzer reproductor de melodías
 * 
 */

static fsm_trans_t fsm_trans_buzzer [] = 
{
    {WAIT_START, check_player_start, WAIT_NOTE, do_player_start},
    {WAIT_NOTE, check_note_end, PLAY_NOTE, do_note_end},
    {PLAY_NOTE, check_pause, PAUSE_NOTE, do_pause},
    {PLAY_NOTE, check_player_stop, WAIT_START, do_player_stop},
    {PLAY_NOTE, check_end_melody, WAIT_MELODY, do_end_melody},
    {PLAY_NOTE, check_play_note, WAIT_NOTE, do_play_note},
    {WAIT_MELODY, check_melody_start, WAIT_NOTE, do_melody_start},
    {PAUSE_NOTE, check_resume, PLAY_NOTE, NULL},
    {-1, NULL, -1, NULL}
};

/* Public functions */

/**
* @brief Check if the buzzer finite state machine is playing a melody.
* 
* @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
*/

bool fsm_buzzer_check_activity (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    return (p_fsm->user_action == PLAY);
}

/**
 * @brief This function sets the melody to play. The user must pass a pointer to the melody to play.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @param p_melody Pointer to the melody to play
 */

void fsm_buzzer_set_melody (fsm_t *p_this, const melody_t *p_melody){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    p_fsm->p_melody = (melody_t*)p_melody;
}

/**
 * @brief This function sets the speed of the player. The user must pass a double value with the speed of the player.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @param speed Speed of the player
 * 
 */

void fsm_buzzer_set_speed (fsm_t *p_this, double speed){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    p_fsm->player_speed = speed;
}

/**
 * @brief This function sets the action to perform on the player. The user must pass a USER_ACTIONS value with the action desired. 
 * These serve as flags to indicate if the user has stopped, paused or started the player, or if the player has stopped itself.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @param action Action to perform on the player.
 * 
 */

void fsm_buzzer_set_action (fsm_t *p_this, uint8_t action){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    p_fsm->user_action = action;
    if (action == STOP){
        p_fsm->note_index = 0;
    }
}

/**
 * @brief This function returns the action performed on the player. This is useful to know if the player is playing, paused or stopped and a melody has ended.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * 
 */

uint8_t fsm_buzzer_get_action (fsm_t *p_this){
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    return p_fsm->user_action;
}

/**
 * @brief Creates a new buzzer finite state machine.
 * 
 * @param buzzer_id Unique buzzer identifier number.
 * @return fsm_t* 
 */

fsm_t *fsm_buzzer_new(uint32_t buzzer_id)
{
    fsm_t *p_fsm = malloc(sizeof(fsm_buzzer_t));
    fsm_buzzer_init(p_fsm, buzzer_id);
    return p_fsm;
}

/**
 * @brief This function initializes the default values of the FSM struct and calls to the port to initialize the HW of associated to the given ID.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t.
 * @param buzzer_id Unique buzzer identifier number.
 */

void fsm_buzzer_init(fsm_t *p_this, uint32_t buzzer_id)
{
    fsm_buzzer_t *p_fsm = (fsm_buzzer_t *)(p_this);
    fsm_init(p_this, fsm_trans_buzzer);
    p_fsm->buzzer_id = buzzer_id;
    p_fsm->p_melody = NULL;
    p_fsm->note_index = 0;
    p_fsm->user_action = STOP;
    p_fsm->player_speed = 1.0;
    port_buzzer_init(buzzer_id);
}