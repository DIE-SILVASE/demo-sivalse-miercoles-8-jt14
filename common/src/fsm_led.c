#include <stddef.h>
#include <stdlib.h>
#include "fsm_button.h"
#include "fsm_led.h"
#include "port_led.h"
#include "port_system.h"

static bool check_button_duration(fsm_t *p_fsm)
{
    fsm_led_t *p_led = (fsm_led_t *)p_fsm;
    return fsm_button_get_duration(p_led->p_button) >= p_led->min_duration;
}

static void do_toggle(fsm_t *p_fsm)
{
    fsm_led_t *p_led = (fsm_led_t *)p_fsm;
    fsm_button_reset_duration(p_led->p_button);
    port_led_toggle();
}

static fsm_trans_t fsm_trans_led[] = {
    {IDLE, check_button_duration, IDLE, do_toggle},
    {-1, NULL, -1, NULL}};

fsm_t *fsm_led_new(fsm_t *p_button, uint32_t min_duration)
{
    fsm_t *p_fsm = malloc(sizeof(fsm_led_t));
    if (p_fsm)
    {
        fsm_led_init(p_fsm, p_button, min_duration);
    }
    return p_fsm;
}
void fsm_led_init(fsm_t *p_fsm, fsm_t *p_button, uint32_t min_duration)
{
    fsm_led_t *p_led = (fsm_led_t *)p_fsm;
    fsm_init(&p_led->fsm, fsm_trans_led);
    p_led->p_button = p_button;
    p_led->min_duration = min_duration;
    port_led_gpio_setup(); /* Inicializa el HW del LED */
}
