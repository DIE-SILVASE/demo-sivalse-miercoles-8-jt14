/**
 * @file fsm_usart.c
 * @brief USART FSM main file.
 * @author Eduardo García
 * @author Roberto Antolín
 * @date 7/03/2024
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C libraries */
#include <string.h>
#include <stdlib.h>
/* Other libraries */
#include "port_usart.h"
#include "fsm_usart.h"
/* State machine input or transition functions */

/**
 * @brief Checks if data have been received.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t.
 * @return true
 * @return false
 */

static bool check_data_rx(fsm_t *p_this)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    return port_usart_rx_done(p_fsm->usart_id);
}

/**
 * @brief Checks if there are data to be sent.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t.
 * @return true
 * @return false
 */

static bool check_data_tx(fsm_t *p_this)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    return (p_fsm->out_data[0] != EMPTY_BUFFER_CONSTANT);
}

/**
 * @brief Checks if the data have been sent.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 * @return true
 * @return false
 */

static bool check_tx_end(fsm_t *p_this)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    return port_usart_tx_done(p_fsm->usart_id);
}

/**
 * @brief Gets the data received by the USART that still remains in the internal buffer of the PORT layer.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 */

static void do_get_data_rx(fsm_t *p_this)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    port_usart_get_from_input_buffer(p_fsm->usart_id, p_fsm->in_data); 
    port_usart_reset_input_buffer(p_fsm->usart_id);
    p_fsm->data_received = true;
}

/**
 * @brief Sets the data to be sent by the USART to the internal buffer of the PORT layer.
 * @note The order of write_data and enable_tx_interrupt is important because of how the USART HW and its interrupts work.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 */

static void do_set_data_tx(fsm_t *p_this)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    port_usart_reset_output_buffer(p_fsm->usart_id);
    port_usart_copy_to_output_buffer(p_fsm->usart_id,p_fsm->out_data, USART_OUTPUT_BUFFER_LENGTH);
    while(!port_usart_get_txr_status(p_fsm->usart_id)){}
    port_usart_write_data(p_fsm->usart_id);
    port_usart_enable_tx_interrupt(p_fsm->usart_id);
}

/**
 * @brief Finishes the data transmission by resetting the output data in the PORT layer.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 */

static void do_tx_end(fsm_t *p_this)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    port_usart_reset_output_buffer(p_fsm->usart_id);
    memset(p_fsm->out_data, EMPTY_BUFFER_CONSTANT, USART_OUTPUT_BUFFER_LENGTH);
}

static fsm_trans_t 	fsm_trans_usart [] = 
{
{WAIT_DATA, check_data_tx, SEND_DATA, do_set_data_tx},
{WAIT_DATA, check_data_rx, WAIT_DATA, do_get_data_rx},
{SEND_DATA, check_tx_end, WAIT_DATA, do_tx_end},
{-1, NULL, -1, NULL}
};


/* State machine output or action functions */

/**
 * @brief Check if the USART FSM is active, or not.
 * @note The USART is active either when it is in the state SEND_DATA or there is data to be read (indicated as true in the field data_received).
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 * @return true
 * @return false
 */

bool fsm_usart_check_activity(fsm_t *p_this)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    return (p_fsm->f.current_state == SEND_DATA | p_fsm->data_received == true);

}

/**
 * @brief Checks if data has been received. If so, it returns true and the user can read the data using the function fsm_usart_get_in_data().
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 * @return true
 * @return false
 */

bool fsm_usart_check_data_received(fsm_t *p_this)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    return p_fsm->data_received;
}

/**
 * @brief Disable the USART RX interrupt.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 */

void fsm_usart_disable_rx_interrupt(fsm_t *p_this)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    port_usart_disable_rx_interrupt(p_fsm->usart_id);
}

/**
 * @brief Disable the USART TX interrupts.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 */

void fsm_usart_disable_tx_interrupt(fsm_t *p_this)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    port_usart_disable_tx_interrupt(p_fsm->usart_id);
}

/**
 * @brief Enable the USART RX interrupt.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 */

void fsm_usart_enable_rx_interrupt(fsm_t *p_this)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    port_usart_enable_rx_interrupt(p_fsm->usart_id);
}

/**
 * @brief Enable the USART TX interrupts.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 */

void fsm_usart_enable_tx_interrupt(fsm_t *p_this)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    port_usart_enable_tx_interrupt(p_fsm->usart_id);
}

/**
 * @brief Reset the input data buffer.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 */

void fsm_usart_reset_input_data(fsm_t *p_this)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    memset(p_fsm->in_data, EMPTY_BUFFER_CONSTANT, USART_INPUT_BUFFER_LENGTH);
    p_fsm->data_received = false;
}

/* Public functions */

/**
 * @brief Returns the data received by the USART.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 * @param p_data Pointer to the array where the data will be copied from the in_data array
 */

void fsm_usart_get_in_data(fsm_t *p_this, char *p_data)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    memcpy(p_data, p_fsm->in_data, USART_INPUT_BUFFER_LENGTH);
}

/**
 * @brief Set the data to send.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 * @param p_data Pointer to an array from where the data will be copied to the out_data array
 */

void fsm_usart_set_out_data(fsm_t *p_this, char *p_data)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    // Ensure to reset the output data before setting a new one
    memset(p_fsm->out_data, EMPTY_BUFFER_CONSTANT, USART_OUTPUT_BUFFER_LENGTH);
    memcpy(p_fsm->out_data, p_data, USART_OUTPUT_BUFFER_LENGTH);
}

/**
 * @brief Create a new USART FSM.
 * This FSM implements a USART communication protocol. It is a state machine that sends and receives data.
 * The FSM stores the received data in the in_data array. The user should ask for it using the function fsm_usart_get_in_data().
 * At start and reset, the in_data array must be empty. An empty array means that there has not been new data.
 *
 * @attention The user is required to reset the in_data array once it has been read. Otherwise, this value may be misinterpreted by the user, if successive calls are made without having received new data. In such a case we would be reading past information. In order to reset the value, the function fsm_usart_reset_input_data() must be called.
 *
 * @param usart_id Unique USART identifier number
 * @return fsm_t* A pointer to the USART FSM
 */

fsm_t *fsm_usart_new(uint32_t usart_id)
{
    fsm_t *p_fsm = malloc(sizeof(fsm_usart_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_usart_init(p_fsm, usart_id);
    return p_fsm;
}

/**
 * @brief Initializes the default values of the FSM struct and calls to the port to initialize the HW of associated to the given ID.
 *
 * @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t struct
 * @param usart_id
 */

void fsm_usart_init(fsm_t *p_this, uint32_t usart_id)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    fsm_init(p_this, fsm_trans_usart);
    p_fsm-> usart_id = usart_id;
    p_fsm -> data_received = false; 
    memset(p_fsm->in_data, EMPTY_BUFFER_CONSTANT, USART_INPUT_BUFFER_LENGTH);
    memset(p_fsm->out_data, EMPTY_BUFFER_CONSTANT, USART_OUTPUT_BUFFER_LENGTH);
    port_usart_init (usart_id); /* Initialize the button HW */
}
