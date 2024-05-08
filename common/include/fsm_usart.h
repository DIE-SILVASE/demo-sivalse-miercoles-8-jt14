/**
 * @file fsm_usart.h
 * @brief Header for fsm_usart.c file.
 * @author Eduardo García
 * @author Roberto Antolín
 * @date 7/03/2024
 */

#ifndef FSM_USART_H_
#define FSM_USART_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Other includes */
#include <fsm.h>
#include "port_usart.h"

/* HW dependent includes */

/* Defines and enums ----------------------------------------------------------*/
/* Enums */

enum FSM_USART {
  WAIT_DATA = 0,
  SEND_DATA
};

/* Typedefs --------------------------------------------------------------------*/

typedef struct{
    fsm_t f; /*USART FSM*/
    bool data_received; /*Flag to indicate that a data has been received*/
    char in_data [USART_INPUT_BUFFER_LENGTH]; /*Input data*/
    char out_data [USART_OUTPUT_BUFFER_LENGTH]; /*Output data*/
    uint8_t usart_id; /*USART ID. Must be unique.*/
} fsm_usart_t;

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Create a new USART FSM.
 * This FSM implements a USART communication protocol. It is a state machine that sends and receives data.
 * The FSM stores the received data in the in_data array. The user should ask for it using the function fsm_usart_get_in_data().
 * At start and reset, the in_data array must be empty. An empty array means that there has not been new data.
 *
 * @param usart_id Unique USART identifier number
 * @return fsm_t* A pointer to the USART FSM
 */

fsm_t *fsm_usart_new(uint32_t usart_id);

/**
 * @brief Initializes the default values of the FSM struct and calls to the port to initialize the HW of associated to the given ID.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 * @param usart_id
 */

void fsm_usart_init(fsm_t *p_this, uint32_t usart_id);

/**
 * @brief Checks if data has been received. If so, it returns true and the user can read the data using the function fsm_usart_get_in_data().
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 * @return true
 * @return false
 */

bool fsm_usart_check_data_received(fsm_t *p_this);

/**
 * @brief Returns the data received by the USART.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 * @param p_data Pointer to the array where the data will be copied from the in_data array
 */

void fsm_usart_get_in_data(fsm_t *p_this, char *p_data);

/**
 * @brief Set the data to send.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 * @param p_data Pointer to an array from where the data will be copied to the out_data array
 */

void fsm_usart_set_out_data(fsm_t *p_this, char *p_data);

/**
 * @brief Reset the input data buffer.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 */

void fsm_usart_reset_input_data(fsm_t *p_this);

/**
 * @brief Checks if the USART FSM is active, or not.
 * @note The USART is active either when it is in the state SEND_DATA or there is data to be read (indicated as true in the field data_received).
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 * @return true
 * @return false
 */

bool fsm_usart_check_activity(fsm_t *p_this);

/**
 * @brief Disable the USART RX interrupt.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 */

void fsm_usart_disable_rx_interrupt(fsm_t *p_this);

/**
 * @brief Disable the USART TX interrupts.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 */

void fsm_usart_disable_tx_interrupt(fsm_t *p_this);

/**
 * @brief Enable the USART RX interrupt.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 */

void fsm_usart_enable_rx_interrupt(fsm_t *p_this);

/**
 * @brief Enable the USART TX interrupts.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 */

void fsm_usart_enable_tx_interrupt(fsm_t *p_this);

#endif /* FSM_USART_H_ */
