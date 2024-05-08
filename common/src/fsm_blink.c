/**
 * @file fsm_blink.c
 * @brief Blink LED FSM main file.
 * @author Román Cárdenas, Alberto Boscá, Josué Pagán (r.cardenas@upm.es, j.pagan@upm.es, alberto.bosca@upm.es)
 * @date 2024-01-01
 */
/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h> // POder usar NULL

/* HW dependent includes */
#include "port_system.h" //para librería FSM
#include "port_led.h" // para consultar el tiempo del sistema

/* Other includes */
#include "fsm_blink.h" // para interaccionar con LED

/* State machine input or transition functions */ 
/**
 * @brief Checks if the LED must toggle.
 *
 * @param p_fsm pointer to the blink FSM.
 *
 * > **TO-DO alumnos:**
 * >
 * > ✅ 1. Cast the generic FSM pointer to blink FSM pointer \n
 * > ✅ 2. Check if current system time is greater than or equal to the FSM's last time + half of its period
 *
 * @return true if the LED must toggle, false otherwise
 */
static bool check_timeout(fsm_t *p_fsm)
{
    fsm_blink_t * p_blink = ( fsm_blink_t *) p_fsm ;
    return port_system_get_millis () >= p_blink -> last_time + p_blink -> period_ms / 2;
}

/* State machine output or action functions */
/**
 * @brief Toggles the LED
 *
 * @param p_fsm pointer to the blink FSM.
 *
 * > **TO-DO alumnos:**
 * >
 * > ✅ 1. Cast the generic FSM pointer to blink FSM pointer \n
 * > ✅ 2. Update FSM's last time to current system time \n
 * > ✅ 3. Toggle the LED using the right system function.
 *
 */
static void do_toggle(fsm_t *p_fsm)
{
    fsm_blink_t * p_blink = ( fsm_blink_t *) p_fsm ;
    p_blink -> last_time = port_system_get_millis () ;
    port_led_toggle () ;

}


/**
 * @brief Blink FSM transition table
 *
 * > **TO-DO alumnos:**
 * >
 * > ✅ 1. Define the FSM's only transition for toggling the LED. \n
 * > ✅ 2. Add a null transition (this is mandatory for all the FSMs).
 *
 */
static fsm_trans_t fsm_blink_tt[] = {
    {IDLE , check_timeout , IDLE , do_toggle },
    { -1 , NULL , -1, NULL } ,
};

fsm_t *fsm_blink_new(uint32_t period_ms)
{
    fsm_t *p_fsm = (fsm_t *)malloc(sizeof(fsm_blink_t));
    if (p_fsm)
    {
        fsm_blink_init(p_fsm, period_ms);
    }
    return p_fsm;
}

void fsm_blink_init(fsm_t *p_fsm, uint32_t period_ms)
{
    fsm_blink_t *p_blink = (fsm_blink_t *)p_fsm;
    fsm_init(&p_blink->fsm, fsm_blink_tt); // inicializo la FSM interna
    p_blink -> last_time = port_system_get_millis () ;
    p_blink -> period_ms = period_ms ;
    port_led_gpio_setup () ; // configuro el pin GPIO del LED
    
    
}
