/**
 * @file interr.c
 * @brief Interrupt service routines for the STM32F4 platform.
 * @author Sistemas Digitales II
 * @date 2024-01-01
 */
// Include HW dependencies:
#include "port_system.h"
#include "port_button.h"
#include "port_usart.h"
#include "port_buzzer.h"
//#include "port_buzzer.h"

// Include headers of different port elements:

//------------------------------------------------------
// INTERRUPT SERVICE ROUTINES
//------------------------------------------------------

/**
 * @brief Interrupt service routine for the System tick timer (SysTick).
 * @note This ISR is called when the SysTick timer generates an interrupt. The program flow jumps to this ISR and increments the tick counter by one millisecond.
 */

void SysTick_Handler(){
    port_system_set_millis(port_system_get_millis() + 1);
}

/**
 * @brief This function handles Px10-Px15 global interrupts.
 * 
 * @note First, this function identifies where the interruption has been raised. Then, perform the desired action. Before leaving it cleans the interrupt pending register.
 */

void EXTI15_10_IRQHandler ( void )
{
    port_system_systick_resume();
/* ISR user button */
if (EXTI ->PR & BIT_POS_TO_MASK( buttons_arr[ BUTTON_0_ID ]. pin ))
{
    if(buttons_arr[BUTTON_0_ID].p_port->IDR & BIT_POS_TO_MASK(buttons_arr[ BUTTON_0_ID ]. pin))
        buttons_arr[BUTTON_0_ID].flag_pressed = false;
    else
        buttons_arr[BUTTON_0_ID].flag_pressed = true;
    EXTI->PR |= BIT_POS_TO_MASK(buttons_arr[ BUTTON_0_ID ]. pin);
}
}

/**
 * @brief This function handles USART3 global interrupt.
 * 
 * First, this function identifies where the interruption has been raised. Then, perform the desired action. Before leaving it cleans the interrupt pending register.
 * 
 * The program flow jumps to this ISR when the USART3 generates an interrupt. It can be due to:
 * 
 * Reception of a new byte (RXNE)
 * Transmission of a byte has finished (TC), although for this project we don't contemplate this option
 * Transmission buffer is empty (TXE)
 * 
 */

void USART3_IRQHandler (void)
{
    port_system_systick_resume();
    if((USART3->SR & USART_SR_RXNE) & (USART3->CR1 & USART_CR1_RXNEIE))
    {
        port_usart_store_data(USART_0_ID);
    }
    if((USART3->SR & USART_SR_TXE) & (USART3->CR1 & USART_CR1_TXEIE))
    {
        port_usart_write_data(USART_0_ID);
    }
}

/**
 * @brief This function handles TIM2 global interrupt. This timer is used to control the duration of the note. When the timer expires, it generates an interrupt.
 * 
 */

void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_SR_UIF;
    buzzers_arr[BUTTON_0_ID].note_end = true;
}