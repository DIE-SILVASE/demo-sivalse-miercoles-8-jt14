/**
 * @file port_usart.c
 * @brief Portable functions to interact with the USART FSM library.
 * @author Eduardo García
 * @author Roberto Antolín
 * @date 14/03/2024
 */
/* Includes ------------------------------------------------------------------*/
/* Standard C libraries */
#include <string.h>
#include <stdlib.h>
#include "port_system.h"
#include "port_usart.h"
/* HW dependent libraries */

/* Global variables */

port_usart_hw_t usart_arr [] = {
    [USART_0_ID] = {.p_usart = USART_0, .p_port_tx = USART_0_GPIO_TX, .p_port_rx = USART_0_GPIO_RX, .pin_tx = USART_0_PIN_TX, 
    .pin_rx = USART_0_PIN_RX, .alt_func_tx = USART_0_AF_TX, .alt_func_rx = USART_0_AF_RX,  
    .i_idx = 0, .read_complete = false,  .o_idx = 0, .write_complete = false,}
};

/* Private functions */

/**
 * @brief Reset a buffer to a default value.
 * 
 * @param buffer Pointer to the buffer to be reset
 * @param length Length of the buffer
 */

void _reset_buffer(char *buffer, uint32_t length)
{
    memset(buffer, EMPTY_BUFFER_CONSTANT, length);
}

/* Public functions */

/**
 * @brief Check if a transmission is complete.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 * @return true 
 * @return false 
 */

bool port_usart_tx_done (uint32_t usart_id){
    return usart_arr[usart_id].write_complete;
}

/**
 * @brief Check if a reception is complete.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 * @return true 
 * @return false 
 */

bool port_usart_rx_done (uint32_t usart_id){
    return usart_arr[usart_id].read_complete;
}

/**
 * @brief Get the message received through the USART and store it in the buffer passed as argument.
 * 
 * This function is called from the function do_get_data_rx() of the FSM to store the message received to the buffer of the FSM.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 * @param p_buffer Pointer to the buffer where the message will be stored.
 */

void port_usart_get_from_input_buffer (uint32_t usart_id, char *p_buffer){
    memcpy(p_buffer, usart_arr[usart_id].input_buffer, USART_INPUT_BUFFER_LENGTH);
}

/**
 * @brief Check if the USART is ready to receive a new message.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 * @return true 
 * @return false 
 */

bool port_usart_get_txr_status (uint32_t usart_id){
    USART_TypeDef *p_usart = usart_arr[usart_id].p_usart;
    return (p_usart->SR & USART_SR_TXE);
}

/**
 * @brief Copy the message passed as argument to the output buffer of the USART.
 * 
 * This function is called from the function do_set_data_tx() of the FSM to set the message to send to the USART.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 * @param p_data Pointer to the message to send.
 * @param length Length of the message to send.
 */

void port_usart_copy_to_output_buffer (uint32_t usart_id, char *p_data, uint32_t length){
    _reset_buffer(usart_arr[usart_id].output_buffer, length);
    memcpy(usart_arr[usart_id].output_buffer, p_data, length);
}

/**
 * @brief Reset the input buffer of the USART.
 * 
 * This function is called from the function do_get_data_rx() to reset the input buffer of the USART after the message has been read.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 */

void port_usart_reset_input_buffer (uint32_t usart_id){
    _reset_buffer(usart_arr[usart_id].input_buffer, USART_INPUT_BUFFER_LENGTH);
    usart_arr[usart_id].read_complete = false;
}

/**
 * @brief Reset the output buffer of the USART.
 * 
 * This function is called from the function do_set_data_tx() and do_tx_end() to reset the output buffer of the USART after the message has been sent.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 */

void port_usart_reset_output_buffer (uint32_t usart_id){
    _reset_buffer(usart_arr[usart_id].output_buffer, USART_OUTPUT_BUFFER_LENGTH);
    usart_arr[usart_id].write_complete = false;
}

/**
 * @brief Function to read the data from the USART Data Register and store it in the input buffer.
 * 
 * This function is called from the ISR USART3_IRQHandler() when the RXNE flag is set.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 */

void port_usart_store_data (uint32_t usart_id){
    char char_read = USART3->DR;
    if (char_read != END_CHAR_CONSTANT)
    {
        if(usart_arr[usart_id].i_idx >= USART_INPUT_BUFFER_LENGTH)
        {
            usart_arr[usart_id].i_idx = 0;
            usart_arr[usart_id].input_buffer[usart_arr[usart_id].i_idx] = char_read;
            usart_arr[usart_id].i_idx = usart_arr[usart_id].i_idx+1;
        }
        else
        {
        usart_arr[usart_id].input_buffer[usart_arr[usart_id].i_idx] = char_read;
        usart_arr[usart_id].i_idx = usart_arr[usart_id].i_idx+1;
        }
    }
    else
    {
        usart_arr[usart_id].read_complete = 1;
        usart_arr[usart_id].i_idx = 0;
    }
}

/**
 * @brief Function to write the data from the output buffer to the USART Data Register.
 * 
 * This function is called from the ISR USART3_IRQHandler() when the TXE flag is set.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 */

void port_usart_write_data (uint32_t usart_id){
    uint32_t o_idx = usart_arr[usart_id].o_idx;
    char char_write = usart_arr[usart_id].output_buffer[o_idx];
    if ((o_idx == USART_OUTPUT_BUFFER_LENGTH-1) | (char_write == END_CHAR_CONSTANT))
    {
        USART3->DR = char_write;
        port_usart_disable_tx_interrupt(usart_id);
        usart_arr[usart_id].o_idx = 0;
        usart_arr[usart_id].write_complete = 1;
    }
    else
    {
        if(char_write != EMPTY_BUFFER_CONSTANT)
        {
            USART3->DR = char_write;
            usart_arr[usart_id].o_idx++;
        }
    }
}

/**
 * @brief Disable USART RX interrupt.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 */

void port_usart_disable_rx_interrupt (uint32_t usart_id){
    USART3->CR1 &= ~USART_CR1_RXNEIE;
}

/**
 * @brief Enable USART TX interrupts.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 */

void port_usart_disable_tx_interrupt (uint32_t usart_id){
    USART3->CR1 &= ~USART_CR1_TXEIE;
}

/**
 * @brief Enable USART RX interrupt.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 */

void port_usart_enable_rx_interrupt (uint32_t usart_id){
    USART3->CR1 |= USART_CR1_RXNEIE;
}

/**
 * @brief Enable USART TX interrupts.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 */

void port_usart_enable_tx_interrupt (uint32_t usart_id){
        USART3->CR1 |= USART_CR1_TXEIE;
}

/**
 * @brief Configure the HW specifications of a given USART.
 * 
 * @param usart_id USART ID. This index is used to select the element of the usart_arr[] array
 */


void port_usart_init(uint32_t usart_id)
{
    USART_TypeDef *p_usart = usart_arr[usart_id].p_usart;
    GPIO_TypeDef *p_port_tx = usart_arr[usart_id].p_port_tx;
    GPIO_TypeDef *p_port_rx = usart_arr[usart_id].p_port_rx;
    uint8_t pin_tx = usart_arr[usart_id].pin_tx;
    uint8_t pin_rx = usart_arr[usart_id].pin_rx;
    uint8_t alt_func_tx = usart_arr[usart_id].alt_func_tx;
    uint8_t alt_func_rx = usart_arr[usart_id].alt_func_rx;

    port_system_gpio_config(p_port_tx, pin_tx, GPIO_MODE_ALTERNATE, GPIO_PUPDR_PUP);
    port_system_gpio_config(p_port_rx, pin_rx, GPIO_MODE_ALTERNATE, GPIO_PUPDR_PUP);
    port_system_gpio_config_alternate(p_port_tx, pin_tx, alt_func_tx);
    port_system_gpio_config_alternate(p_port_rx, pin_rx, alt_func_rx);
    if (p_usart == USART3)
    {
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Enable peripheral clock
        USART3->CR1 &= ~USART_CR1_UE; // Disable USART3 to configure the registers
        USART3->CR1 &= ~USART_CR1_M; // Data length to 8 bits
        USART3->CR2 &= ~USART_CR2_STOP; // Stop bit to 1
        USART3->CR1 &= ~USART_CR1_PCE; // Parity bit to no parity
        USART3->CR1 &= ~USART_CR1_OVER8; // Oversampling of 16
        USART3->BRR = 0x682; // Cálculo: 16000000/(8x2x9600) y después calculamos la parte decimal
        USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; // Enable tx and rx
        port_usart_disable_tx_interrupt(usart_id);
        port_usart_disable_rx_interrupt(usart_id);
        p_usart->SR &= ~USART_SR_RXNE; // Clear RXNE flag
        NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
        NVIC_EnableIRQ(USART3_IRQn);
        USART3->CR1 |= USART_CR1_UE;
    }
    _reset_buffer(usart_arr[usart_id].input_buffer, USART_INPUT_BUFFER_LENGTH);
    _reset_buffer(usart_arr[usart_id].output_buffer, USART_OUTPUT_BUFFER_LENGTH);
}