/**
 * @file port_button.h
 * @brief Header for port_button.c file.
 * @author Eduardo García
 * @author Roberto Antolín
 * @date 22/02/2024
 */

#ifndef PORT_BUTTON_H_
#define PORT_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>
#include "port_system.h"

/* HW dependent includes */

/* Defines and enums ----------------------------------------------------------*/
/* Defines */

#define BUTTON_0_ID 0             // Valor numérico natural que será el identificador del botón. Primer botón tendrá asignado el 0
#define BUTTON_0_GPIO GPIOC       // GPIO a la que está conectada el botón de usuario en la placa (A, B o C).
#define BUTTON_0_PIN 13           // Pin/ línea de la GPIO del botón.
#define BUTTON_0_DEBOUNCE_TIME_MS 150  // Tiempo del anti-rebotes del botón en ms.
#define GPIO_MODE_INPUT 0         // Configuración GPIO en modo input
#define GPIO_NOPULL 0             // Configuración GPIO sin pull up ni pull down
#define GPIO_RISING_EDGE 0x01U    // Enables rising edge
#define GPIO_FALLING_EDGE 0x02U   // Enables falling edge
#define GPIO_ENABLE_INT 0x08U     // Enables the interrupt request
#define GPIO_EDGES_AND_INT (GPIO_RISING_EDGE|GPIO_FALLING_EDGE|GPIO_ENABLE_INT)     // Enables rising and falling edges and interrupt request
#define PRIORITY_1 1              // Set priority level to 1
#define SUBPRIORITY_0 0           // Set subpriority level to 0

/* Typedefs --------------------------------------------------------------------*/

typedef struct
{
    GPIO_TypeDef *p_port;
    uint8_t pin;
    bool flag_pressed;
} port_button_hw_t;

/* Global variables */

/**
 * @brief Array of elements that represents the HW characteristics of the buttons. Defined in port_button.c
 */
extern port_button_hw_t buttons_arr[];

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Configure the HW specifications of a given button.
 *
 * @param button_id Button ID. This index is used to select the element of the buttons_arr[] array.
 */

void port_button_init(uint32_t button_id);

/**
 * @brief Return the status of the button (pressed or not)
 *
 * @param button_id Button ID. This index is used to select the element of the buttons_arr[] array
 *
 * @return true if the button has been pressed
 * @return false if the button hasn't been pressed
 */

bool port_button_is_pressed(uint32_t button_id);

/**
 * @brief Return the count of the System tick in ms.
 *
 * @return uint32_t
 */

uint32_t port_button_get_tick();

#endif
