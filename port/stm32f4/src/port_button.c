/**
 * @file port_button.c
 * @brief File containing functions related to the HW of the button.
 *
 * This file defines an internal struct which contains the HW information of the button.
 *
 * @author Eduardo García Bermúdez
 * @author Roberto Antolín
 * @date 22/02/2024
 */

/* Includes ------------------------------------------------------------------*/
#include "port_button.h"

/* Global variables ------------------------------------------------------------*/

/**
 * @brief Array of elements that represents the HW characteristics of the buttons. Defined in port_button.c
 */

port_button_hw_t buttons_arr[] = {
    [BUTTON_0_ID] = {.p_port = BUTTON_0_GPIO, .pin = BUTTON_0_PIN, .flag_pressed = false},
};

/*Functions -------------------------------------------------------------*/

/**
 * @brief Configure the HW specifications of a given button.
 * 
 * @param button_id Button ID. This index is used to select the element of the buttons_arr[] array
 */

void port_button_init(uint32_t button_id)
{
    GPIO_TypeDef *p_port = buttons_arr[button_id].p_port;
    uint8_t pin = buttons_arr[button_id].pin;

    port_system_gpio_config(p_port, pin, GPIO_MODE_INPUT, GPIO_NOPULL);
    port_system_gpio_config_exti(p_port, pin, GPIO_EDGES_AND_INT);
    port_system_gpio_exti_enable(pin, PRIORITY_1, SUBPRIORITY_0);
}

/**
 * @brief Return the status of the button (pressed or not)
 * 
 * @param button_id Button ID. This index is used to select the element of the buttons_arr[] array
 * @return true If the button has been pressed
 * @return false If the button has not been pressed
 */

bool port_button_is_pressed(uint32_t button_id)
{
    return buttons_arr[button_id].flag_pressed;
}

/**
 * @brief Return the count of the System tick in milliseconds (ms).
 * 
 * @return uint32_t 
 */

uint32_t port_button_get_tick()
{
    return port_system_get_millis();
}
