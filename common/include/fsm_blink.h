#ifndef FSM_BLINK_H_
#define FSM_BLINK_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>

/* Other includes */
#include <fsm.h>

/* Typedefs --------------------------------------------------------------------*/
/**
 * @enum FSM_BLINK_STATES
 * @brief Blink FSM state enumeration
 */
enum FSM_BLINK_STATES
{
    IDLE, /*!< Idle state */
};

/**
 * @brief Estructura de la máquina de estados para hacer parpadear el LED.
 *
 * Al contrario que en Java o Python, C no implementa un mecanismo de programación orientado a objetos.
 * Por lo tanto, no tenemos ni clases, ni objetos, ni herencia.
 * Sin embargo, podemos usar el patrón de diseño por composición para emular la herencia.
 *
 * Para ello, el primer elemento de la estructura que "hereda" será del tipo de la estructura "padre".
 * En este caso, nuestra FSM heredaría de la estructura fsm_t.
 *
 * Como el primer elemento de nuestra estructura es del tipo fsm_t,
 * podemos tratar punteros a estructuras fsm_blink_t como si fuesen punteros a estructuras fsm_t.
 * De este modo, podemos hacer uso de las funciones de fsm_t con un puntero del tipo `fsm_blink_t`.
 */
typedef struct fsm_blink_t
{
    fsm_t fsm;          //!< inner FSM. It must be the first element so we can use composition.
    uint32_t period_ms; //!< LED toggling period.
    uint32_t last_time; //!< Auxiliary variable to know when was the last time the LED toggled.
} fsm_blink_t;

/* Function prototypes and explanation -------------------------------------------------*/
/**
 * @brief Creates a new FSM for blinking the LED of the board.
 *
 * @note This function uses malloc to save memory space in the heap for the FSM.
 * @note If you are done with the FSM, you must call fsm_destroy to free memory.
 *
 * @param period_ms period (in ms) of the LED blink.
 *
 * @return fsm_t* pointer to the LED FSM.
 */
fsm_t *fsm_blink_new(uint32_t period_ms);

/**
 * @brief Initializes all the parameters for an FSM that blinks the LED of the board.
 *
 * > **TO-DO alumnos:**
 * >
 * > ✅ 1. Cast pointer to blink FSM (we already provide this) \n
 * > ✅ 2. Initialize its inner FSM (we already provide this) \n
 * > ✅ 3. Fill all the fields of the blink FSM to their initial value \n
 * > ✅ 4. Configure the GPIO of the LED accordingly.
 *
 * @param p_fsm pointer to the FSM.
 * @param period_ms blinking period of the LED.
 */
void fsm_blink_init(fsm_t *p_fsm, uint32_t period_ms);

#endif // FSM_BLINK_H_
