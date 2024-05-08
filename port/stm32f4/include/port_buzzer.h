/**
 * @file port_buzzer.h
 * @brief Header for port_buzzer.c file.
 * @author Eduardo García
 * @author Roberto Antolín
 * @date 11/04/2024
 */
#ifndef PORT_BUZZER_H_
#define PORT_BUZZER_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */

#include <stdint.h>
#include <stdbool.h>

/* HW dependent includes */

#include "port_system.h"

/* Defines and enums ----------------------------------------------------------*/
/* Defines */

#define BUZZER_0_ID 0 /*Buzzer melody player identifier*/
#define BUZZER_0_GPIO GPIOA /*Buzzer melody player GPIO port*/
#define BUZZER_0_PIN 6 /*Buzzer melody player GPIO pin*/
#define BUZZER_PWM_DC 0.5 /*PWM duty cycle 0-1*/

/* Typedefs --------------------------------------------------------------------*/

typedef struct {
    GPIO_TypeDef * p_port; /*GPIO where the buzzer melody player is connected*/
    uint8_t pin; /*Pin/line where the buzzer melody player is connected*/
    uint8_t alt_func; /*Alternate function value for PWM according to the Alternate function table of the datasheet*/
    bool note_end; /*Flag to indicate that the note has ended*/
}port_buzzer_hw_t;

/* Global variables */

extern port_buzzer_hw_t buzzers_arr [];

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Configure the HW specifications of a given buzzer melody player.
 * 
 * @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
 */

void port_buzzer_init (uint32_t buzzer_id);

/**
 * @brief Set the duration of the timer that controls the duration of the note.
 * 
 * @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
 * @param duration_ms Duration of the note in ms
 */
 
void port_buzzer_set_note_duration (uint32_t buzzer_id, uint32_t duration_ms);

/**
 * @brief Set the PWM frequency of the timer that controls the frequency of the note.
 * 
 * @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
 * @param frequency_hz Frequency of the note in Hz
 */

void port_buzzer_set_note_frequency (uint32_t buzzer_id, double frequency_hz);

/**
 * @brief Retrieve the status of the note end flag.
 * 
 * @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
 * @return true 
 * @return false 
 */
 
bool port_buzzer_get_note_timeout (uint32_t buzzer_id);

/**
 * @brief Disable the PWM output of the timer that controls the frequency of the note and the timer that controls the duration of the note.
 * 
 * @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
 */
 
void port_buzzer_stop (uint32_t buzzer_id);

#endif