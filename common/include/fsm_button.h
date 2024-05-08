/**
 * @file fsm_button.h
 * @brief Header for fsm_button.c file.
 * @author Eduardo García
 * @author Roberto Antolín
 * @date 15/02/2024
 */

#ifndef FSM_BUTTON_H_
#define FSM_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Other includes */
#include "fsm.h"

/* Defines and enums ----------------------------------------------------------*/
/* Enums */
enum FSM_BUTTON
{
    BUTTON_RELEASED = 0, /*!< Starting state*/
    BUTTON_RELEASED_WAIT ,  /*!< Starting state*/
    BUTTON_PRESSED ,    /*!< Pressed state*/
    BUTTON_PRESSED_WAIT ,   /*!< Starting state*/

};

/* Typedefs --------------------------------------------------------------------*/
typedef struct
{
    fsm_t f;              /*!< Internal FSM from the library */
    uint32_t debounce_time; /*!< Button debounce time in ms */
    uint32_t next_timeout;  /*!< Next timeout for the debounce in ms */
    uint32_t tick_pressed;  /*!< Number of system ticks when the button was pressed */
    uint32_t duration;      /*!< How much time the button has been pressed */
    uint32_t button_id;
} fsm_button_t;

/* Function prototypes and documentation ---------------------------------------*/



/**
 * @brief Creates a new FSM for measuring how long the button is pressed.
 *
 * @note this function uses malloc to save memory space in the heap for the FSM.
 * @note If you are done with the FSM, you must call fsm_destroy to free memory.
 *
 * @param debounce_time time (in ms) the FSM will wait in intermediate steps to avoid mechanical gltiches.
 *
 * @return fsm_t* pointer to the button FSM.
 */
fsm_t * fsm_button_new(uint32_t debounce_time, uint32_t button_id);

/**
 * @brief Initializes all the parameters for an FSM that measures how long the button is pressed.
 *
 * @param p_this pointer to the FSM.
 * @param debounce_time time (in ms) the FSM will wait in intermediate steps to avoid mechanical gltiches.
 */
void fsm_button_init(fsm_t *p_this, uint32_t debounce_time, uint32_t button_id);

/**
 * @brief Returns the latest duration measured by the button FSM.
 *
 * @param p_this pointer to the button FSM.
 * @return uint32_t amount of time (in ms) that the button has been pressed.
 */
uint32_t fsm_button_get_duration(fsm_t *p_this);

/**
 * @brief Sets the duration measured by the button FSM to 0.
 * 
 * @param p_fsm pointer to the button FSM.
 */
void fsm_button_reset_duration(fsm_t *p_this);

/**
 * @brief Check if the button FSM is active, or not. The button is inactive when it is in the status BUTTON_RELEASED.
 * 
 * @param p_this pointer to the button FSM.
 * 
 * @return true 
 * @return false 
 */
bool fsm_button_check_activity (fsm_t *p_this);

#endif
