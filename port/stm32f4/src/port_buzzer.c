/**
 * @file port_buzzer.c
 * @brief Portable functions to interact with the Buzzer melody player FSM library.
 * @author Eduardo García
 * @author Roberto Antolín
 * @date 11/04/2024
 */
/* Includes ------------------------------------------------------------------*/
/* Standard C libraries */
#include <math.h>
/* HW dependent libraries */
#include "port_buzzer.h"
/* Global variables */

#define ALT_FUNC2_TIM3 2    /*TIM3 Alternate Function mapping*/

port_buzzer_hw_t buzzers_arr[] = 
{
  [BUZZER_0_ID] = {.p_port = BUZZER_0_GPIO, .pin = BUZZER_0_PIN, .alt_func = ALT_FUNC2_TIM3, .note_end = true,}
};

/* Private functions */

/**
 * @brief Configure the timer that controls the duration of the note.
 * 
 * @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
 */

static void _timer_duration_setup(uint32_t buzzer_id)
{
  if (buzzer_id == BUZZER_0_ID)
  {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->CR1 |= TIM_CR1_ARPE;
    TIM2->SR = ~TIM_SR_UIF;
    TIM2->DIER |= TIM_DIER_UIE;

    /* Configure interruptions */
    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 3, 0)); 
    NVIC_EnableIRQ(TIM2_IRQn);                                                          
  }
}

/**
 * @brief Configure the timer that controls the PWM of the buzzer.
 * 
 * @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
 */

static void _timer_pwm_setup (uint32_t buzzer_id){
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  TIM3->CR1 &= ~TIM_CR1_CEN;
  TIM3->CR1 |= TIM_CR1_ARPE;
  TIM3->CNT = 0;
  TIM3->ARR = 0;
  TIM3->PSC = 0;
  TIM3->EGR = TIM_EGR_UG;
  TIM3->CCER &= ~TIM_CCER_CC1E;
  TIM3->CCMR1 |= 0x0060; /* Modo PWM 1 */
  TIM3->CCMR1 |= TIM_CCMR1_OC1PE ;
}

/* Public functions -----------------------------------------------------------*/

/**
 * @brief Set the duration of the timer that controls the duration of the note.
 * 
 * @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
 * @param duration_ms Duration of the note in ms
 */
 
void port_buzzer_set_note_duration (uint32_t buzzer_id, uint32_t duration_ms){
  /* ARR = (fCLK*TINTERR)/(PSC+1) - 1 */ 
  /* ARR = (SystemCoreClock*duration_ms)/(PSC + 1) -1*/
  TIM2->CR1 &= ~TIM_CR1_CEN;
  TIM2->CNT = 0;
  double sysclk_as_double = (double)SystemCoreClock; /* fCLK = SystemCoreClock */ 
  double s_as_double = (double)(duration_ms)/1000.0;
  double PSC_provisional = (((sysclk_as_double*s_as_double)/65536.0) - 1.0); // Calculo PSC
  double PSC = round(PSC_provisional); // Redondeo normal, pero conlleva error
  double ARR_provisional = round(((sysclk_as_double*s_as_double)/(PSC+1.0) - 1.0)); // Calculamos ARR con el prescaler
  if (ARR_provisional > 65535.0){ // Si el error hacer¡ que ARR desborde, corregimos
    PSC = PSC + 1.0;
    ARR_provisional = round(((sysclk_as_double*s_as_double)/(PSC+1.0) - 1.0));
  }
  TIM2->ARR = (uint32_t)(ARR_provisional);
  TIM2->PSC = (uint32_t)(PSC);
  TIM2->EGR = TIM_EGR_UG;
  buzzers_arr[buzzer_id].note_end = false;
  TIM2->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief Set the PWM frequency of the timer that controls the frequency of the note.
 * 
 * @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
 * @param frequency_hz Frequency of the note in Hz
 */

void port_buzzer_set_note_frequency (uint32_t buzzer_id, double frequency_hz){
  if (frequency_hz == 0.0){
    TIM3->CR1 &= ~TIM_CR1_CEN;
  } else{
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->CNT = 0;
    double sysclk_as_double = (double)SystemCoreClock; /* fCLK = SystemCoreClock */ 
    double s_as_double = 1.0/frequency_hz;
    double PSC_provisional = (((sysclk_as_double*s_as_double)/65536.0) - 1.0); // Calculo PSC
    double PSC = round(PSC_provisional); // Redondeo normal, pero conlleva error
    double ARR_provisional = round(((sysclk_as_double*s_as_double)/(PSC+1.0) - 1.0)); // Calculamos ARR con el prescaler
    if (ARR_provisional > 65535.0){ // Si el error hacer¡ que ARR desborde, corregimos
      PSC = PSC + 1.0;
      ARR_provisional = round(((sysclk_as_double*s_as_double)/(PSC+1.0) - 1.0));
    } 
    TIM3->ARR = (uint32_t)(ARR_provisional);
    TIM3->PSC = (uint32_t)(PSC);
    TIM3->CCER &= ~TIM_CCER_CC1E;
    TIM3->CCR1 =(uint32_t) round(BUZZER_PWM_DC*(ARR_provisional+1.0));
    TIM3->EGR = TIM_EGR_UG;
    TIM3->CCER |= TIM_CCER_CC1E;
    TIM3->CR1 |= TIM_CR1_CEN;
  }
}

/**
 * @brief Retrieve the status of the note end flag.
 * 
 * @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
 * @return true 
 * @return false 
 */
 
bool port_buzzer_get_note_timeout (uint32_t buzzer_id){
  if(buzzer_id == BUZZER_0_ID){ 
    return buzzers_arr[buzzer_id].note_end;
  } else {
    return false;
  }
}

/**
 * @brief Disable the PWM output of the timer that controls the frequency of the note and the timer that controls the duration of the note.
 * 
 * @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
 */

void port_buzzer_stop (uint32_t buzzer_id){
  if(buzzer_id == BUZZER_0_ID){ 
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM3->CR1 &= ~TIM_CR1_CEN;
  };
}

/**
 * @brief Configure the HW specifications of a given buzzer melody player.
 * 
 * @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
 */

void port_buzzer_init(uint32_t buzzer_id)
{
  port_buzzer_hw_t buzzer = buzzers_arr[buzzer_id];
  GPIO_TypeDef * p_port = buzzer.p_port;
  uint8_t pin = buzzer.pin;
  uint8_t alt_func = buzzer.alt_func;
  port_system_gpio_config(p_port, pin, GPIO_MODE_ALTERNATE, GPIO_PUPDR_NOPULL);
  port_system_gpio_config_alternate(p_port, pin, alt_func);
  _timer_duration_setup(buzzer_id);
  _timer_pwm_setup(buzzer_id);
}
