#include "stm32l1xx_hal.h"
#include "main.h"
#include "pwr_hal.h"

typedef enum POWER_TYPE {
    POWER_TYPE_START                = 0,
    POWER_5V                        = 0,           
    POWER_3V3,          
    POWER_3V3_BLE,
    POWER_3V3_SN595,
    POWER_3V3_FP,
    POWER_TYPE_END                  
}power_typedef_e;

typedef enum POWER_STATE {
    POWER_ON = 0,
    POWER_OFF
}power_state_e;

typedef struct POWER_HAL_PARAM {
    GPIO_TypeDef *          port_addr;
    uint16_t                number;
    GPIO_PinState           state[2];       /** !< {POWER_ON, POWER_OFF} */
}power_pin_param_t;

typedef struct POWER_HANDLER {
    uint8_t                 pin_latest_state;
    power_pin_param_t       pin_param;
    void                    (*pin_set_func)(power_typedef_e power_type, power_state_e power_pin_state);
//    void                    (*power_on) (power_typedef_e power_type);
//    void                    (*power_off) (power_typedef_e power_type);
}power_handler_t;

static const power_pin_param_t power_pin_param[POWER_TYPE_END] = {
    { POWER_ENABLE_5V_GPIO_Port,     POWER_ENABLE_5V_Pin,    {GPIO_PIN_SET, GPIO_PIN_RESET} },
    { POWER_ENABLE_3V3_GPIO_Port,    POWER_ENABLE_3V3_Pin,   {GPIO_PIN_SET, GPIO_PIN_RESET} },
    { POWER_ENABLE_BLE_GPIO_Port,    POWER_ENABLE_BLE_Pin,   {GPIO_PIN_RESET, GPIO_PIN_SET} },
    { POWER_ENABLE_SN595_GPIO_Port,  POWER_ENABLE_SN595_Pin, {GPIO_PIN_RESET, GPIO_PIN_SET} },
    { POWER_ENABLE_FP_GPIO_Port,     POWER_ENABLE_FP_Pin,    {GPIO_PIN_RESET, GPIO_PIN_SET} },
};

static void power_switch (power_typedef_e power_type, power_state_e power_pin_state);
static bool power_good_check (void);
//static void power_on (power_typedef_e power_type);
//static void power_off (power_typedef_e power_type);

static power_handler_t power_handler[POWER_TYPE_END];

void power_init (void) {
    uint8_t index = 0;
    power_state_e power_state;
    
    for (index = 0; index < POWER_TYPE_END; index++) {
        power_handler[index].pin_latest_state = POWER_OFF;
        memcpy ((char *)&power_handler[index].pin_param.port_addr, (char *)&power_pin_param[index].port_addr, sizeof(power_pin_param_t));
        power_handler[index].pin_set_func = power_switch;
    }
    
    power_handler[POWER_5V ].pin_set_func(POWER_5V, POWER_ON);
    power_handler[POWER_3V3].pin_set_func(POWER_3V3, POWER_ON);
    
    if ( power_good_check() ) {
        power_state = POWER_ON;
        power_handler[POWER_3V3_SN595].pin_set_func(POWER_3V3_SN595, POWER_ON);
        power_handler[POWER_3V3_FP].pin_set_func(POWER_3V3_FP, POWER_ON);
        power_handler[POWER_3V3_BLE].pin_set_func(POWER_3V3_BLE, POWER_ON);
    } else {
        power_state = POWER_OFF;
    }
    power_handler[POWER_5V ].pin_latest_state = power_state;
    power_handler[POWER_3V3].pin_latest_state = power_state;   
}

static bool power_good_check (void) {
    uint32_t tick_start = HAL_GetTick();
    bool status = true;
    
//    while (!HAL_GPIO_ReadPin(EI_3V3_PG_GPIO_Port, EI_3V3_PG_Pin) && \
//           !HAL_GPIO_ReadPin(EI_5V_PG_GPIO_Port, EI_5V_PG_Pin)) {
//        if (HAL_GetTick() - tick_start >= 20 ) {
//            status = false;
//            break;
//        }
//    }
    
    return status;
}

static void power_switch (power_typedef_e power_type, power_state_e pin_state) {
    power_typedef_e epower;
    
    epower = power_type;
    HAL_GPIO_WritePin(power_pin_param[epower].port_addr, \
                      power_pin_param[epower].number, \
                      power_pin_param[epower].state[(uint8_t)pin_state]);
    
    power_handler[epower].pin_latest_state = pin_state;
}

//static void power_on (power_typedef_e power_type) {
//    power_typedef_e epower;
//    
//    epower = power_type;
//    HAL_GPIO_WritePin(power_pin_param[epower].port_addr, \
//                      power_pin_param[epower].number, \
//                      power_pin_param[epower].state[0]);
//    
//    power_handler[epower].pin_latest_state = POWER_ON;
//}

//static void power_off (power_typedef_e power_type) {
//    power_typedef_e epower;
//    
//    epower = power_type;
//    HAL_GPIO_WritePin(power_pin_param[epower].port_addr, \
//                      power_pin_param[epower].number, \
//                      power_pin_param[epower].state[1]);
//    
//    power_handler[epower].pin_latest_state = POWER_OFF;
//}

