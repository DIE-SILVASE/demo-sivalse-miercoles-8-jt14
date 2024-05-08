/**
 * @file fsm_button.c
 * @brief Button FSM main file.
 * @author Eduardo García
 * @author Roberto Antolín
 * @date 15/02/2024
 */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "fsm_button.h"
#include "port_button.h"

/* State machine input or transition functions */
/**
 * @brief Checks if the button has been pressed.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
 *
 * @return true
 * @return false
 */

static bool check_button_pressed(fsm_t *p_this)
{
    fsm_button_t *p_button = (fsm_button_t *)(p_this);
    bool check_pressed = port_button_is_pressed(p_button->button_id);
    return check_pressed;
}

/**
 * @brief Checks if the button is released.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
 *
 * @return true if the button is realeased; false otherwise.
 */

static bool check_button_released(fsm_t *p_this)
{
    fsm_button_t *p_button = (fsm_button_t *)(p_this);
    bool check_pressed = port_button_is_pressed(p_button->button_id);
    return !check_pressed;
}

/**
 * @brief Checks if the debounce time has passed.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
 *
 * @return true if the current system time is greater than the last debounce timeout.
 */

static bool check_timeout(fsm_t *p_this)
{
    fsm_button_t *p_button = (fsm_button_t *)p_this;
    uint32_t now = port_button_get_tick();
    return now > p_button->next_timeout;
}

/* State machine output or action functions */

/**
 * @brief Store the system tick when the button was pressed.
 *
 * @param p_this pointer to an fsm_t struct than contains an fsm_button_t.
 */

static void do_store_tick_pressed(fsm_t *p_this)
{
    fsm_button_t *p_button = (fsm_button_t *)p_this;
    uint32_t now = port_button_get_tick();

    p_button->tick_pressed = now;
    p_button->next_timeout = now + p_button->debounce_time;
}

/**
 * @brief Store the duration of the button press.
 *
 * @param p_this pointer to an fsm_t struct than contains an fsm_button_t.
 */

static void do_set_duration(fsm_t *p_this)
{
    fsm_button_t *p_button = (fsm_button_t *)p_this;
    uint32_t now = port_button_get_tick();

    p_button->duration = now - p_button->tick_pressed;
    p_button->next_timeout = now + p_button->debounce_time;
}

/**
 * @brief Array representing the transitions table of the FSM button.
 *
 */

static fsm_trans_t fsm_trans_button[] = // {EstadoIni , FuncCompruebaCondicion, EstadoSig, FuncAccionesSiTransicion}
    {
        {BUTTON_RELEASED, check_button_pressed, BUTTON_PRESSED_WAIT, do_store_tick_pressed},
        {BUTTON_PRESSED_WAIT, check_timeout, BUTTON_PRESSED, NULL},
        {BUTTON_PRESSED, check_button_released, BUTTON_RELEASED_WAIT, do_set_duration},
        {BUTTON_RELEASED_WAIT, check_timeout, BUTTON_RELEASED, NULL},
        {-1, NULL, -1, NULL}

};
/* State machine output or action functions */

/**
 * @brief Returns the duration of the last button press.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
 * 
 * @return uint32_t 
 */

uint32_t fsm_button_get_duration(fsm_t *p_this)
{
    fsm_button_t *p_button = (fsm_button_t *)p_this;
    return p_button->duration;
}

/**
 * @brief Reset the duration of the last button press.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
 */

void fsm_button_reset_duration(fsm_t *p_this)
{
    fsm_button_t *p_button = (fsm_button_t *)p_this;
    p_button->duration = 0;
}

/**
 * @brief Checks if the button FSM is active, or not. The button is inactive when it is in the status BUTTON_RELEASED.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
 * 
 * @return true 
 * @return false 
 */

bool fsm_button_check_activity(fsm_t *p_this)
{
    fsm_button_t *p_button = (fsm_button_t *)p_this;
    return !(p_button->f.current_state == BUTTON_RELEASED);
}

/* Other auxiliary functions */

/**
 * @brief Creates a new button FSM.
 * 
 * @param debounce_time Anti-debounce time in milliseconds
 * @param button_id Unique button identifier number
 * 
 * @return fsm_t* 
 */

fsm_t *fsm_button_new(uint32_t debounce_time, uint32_t button_id)
{
    fsm_t *p_this = malloc(sizeof(fsm_button_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_button_reset_duration(p_this);
    fsm_button_init(p_this, debounce_time, button_id);
    return p_this;
}

/**
 * @brief Initializes the default values of the FSM struct and calls to the port to initialize the HW of associated to the given ID.
 * 
 * @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
 * @param debounce_time Anti-debounce time in milliseconds
 * @param button_id Unique button identifier number
 */

void fsm_button_init(fsm_t *p_this, uint32_t debounce_time, uint32_t button_id)
{
    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);
    fsm_init(p_this, fsm_trans_button);
    p_fsm-> debounce_time = debounce_time ;
    p_fsm -> tick_pressed = 0;
    p_fsm -> duration = 0;
    p_fsm -> button_id = button_id;
    port_button_init (button_id); /* Initialize the button HW */
}
