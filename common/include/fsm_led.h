#ifndef FSM_LED_H_
#define FSM_LED_H_

#include <stdint.h>
#include <fsm.h>

enum FSM_LED_STATES
{
IDLE
};

typedef struct
{
fsm_t fsm ; /*! < Internal FSM from the library */
fsm_t * p_button ; /*! < Pointer to button FSM */
uint32_t min_duration ; /*! < Minimum button pulse for toggling the LED */
} fsm_led_t ;

fsm_t * fsm_led_new ( fsm_t * p_button , uint32_t min_duration );
void fsm_led_init ( fsm_t *p_fsm , fsm_t * p_button , uint32_t min_duration );

# endif // FSM_LED_H_