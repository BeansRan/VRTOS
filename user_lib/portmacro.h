#ifndef PORTMACRO_INCLUDE
#define PORTMACRO_INCLUDE

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "stm32l1xx.h"
#include "stm32l1xx_hal.h"

#include "app_trace.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief Implementation specific macro for delayed macro expansion used in string concatenation
*
* @param[in]   lhs   Left hand side in concatenation
* @param[in]   rhs   Right hand side in concatenation
*/
#define STRING_CONCATENATE_IMPL(lhs, rhs) lhs ## rhs


/**@brief Macro used to concatenate string using delayed macro expansion
*
* @note This macro will delay concatenation until the expressions have been resolved
*
* @param[in]   lhs   Left hand side in concatenation
* @param[in]   rhs   Right hand side in concatenation
*/
#define STRING_CONCATENATE(lhs, rhs) STRING_CONCATENATE_IMPL(lhs, rhs)
    
/**@brief Macro for doing static (i.e. compile time) assertion.
*
* @note If the EXPR isn't resolvable, then the error message won't be shown.
*
* @note The output of STATIC_ASSERT_MSG will be different across different compilers.
*
* @param[in] EXPR Constant expression to be verified.
*/
#if defined ( __COUNTER__ )

#define STATIC_ASSERT(EXPR) \
    ;enum { STRING_CONCATENATE(static_assert_, __COUNTER__) = 1/(!!(EXPR)) }

#else

#define STATIC_ASSERT(EXPR) \
    ;enum { STRING_CONCATENATE(assert_line_, __LINE__) = 1/(!!(EXPR)) }

#endif

#define APP_ERROR_CHECK( func_name, err_code ) {   \
    LOG ("func = %s error_code = %d", func_name, err_code );    \
    while (1);  \
}    

/** @defgroup ERRORS_BASE Error Codes Base number definitions
 * @{ */
#define ERROR_BASE_NUM      (0x0)       ///< Global error base

#define ERROR_NONE                          (ERROR_BASE_NUM + 0)    ///< Successful command
#define ERROR_SVC_HANDLER_MISSING           (ERROR_BASE_NUM + 1)    ///< SVC handler is missing
#define ERROR_SOFTDEVICE_NOT_ENABLED        (ERROR_BASE_NUM + 2)    ///< SoftDevice has not been enabled
#define ERROR_INTERNAL                      (ERROR_BASE_NUM + 3)    ///< Internal Error
#define ERROR_NO_MEM                        (ERROR_BASE_NUM + 4)    ///< No Memory for operation
#define ERROR_NOT_FOUND                     (ERROR_BASE_NUM + 5)    ///< Not found
#define ERROR_NOT_SUPPORTED                 (ERROR_BASE_NUM + 6)    ///< Not supported
#define ERROR_INVALID_PARAM                 (ERROR_BASE_NUM + 7)    ///< Invalid Parameter
#define ERROR_INVALID_STATE                 (ERROR_BASE_NUM + 8)    ///< Invalid state, operation disallowed in this state
#define ERROR_INVALID_LENGTH                (ERROR_BASE_NUM + 9)    ///< Invalid Length
#define ERROR_INVALID_FLAGS                 (ERROR_BASE_NUM + 10)   ///< Invalid Flags
#define ERROR_INVALID_DATA                  (ERROR_BASE_NUM + 11)   ///< Invalid Data
#define ERROR_DATA_SIZE                     (ERROR_BASE_NUM + 12)   ///< Invalid Data size
#define ERROR_TIMEOUT                       (ERROR_BASE_NUM + 13)   ///< Operation timed out
#define ERROR_NULL                          (ERROR_BASE_NUM + 14)   ///< Null Pointer
#define ERROR_FORBIDDEN                     (ERROR_BASE_NUM + 15)   ///< Forbidden Operation
#define ERROR_INVALID_ADDR                  (ERROR_BASE_NUM + 16)   ///< Bad Memory Address
#define ERROR_BUSY                          (ERROR_BASE_NUM + 17)   ///< Busy
#define ERROR_CONN_COUNT                    (ERROR_BASE_NUM + 18)   ///< Maximum connection count exceeded.
#define ERROR_RESOURCES                     (ERROR_BASE_NUM + 19)   ///< Not enough resources for operation

/**@brief Byte array type. */
#define FALSE                               0
#define TRUE                                1
#define DUMMY_BYTE                          0xA2

typedef struct
{
    uint16_t  size;                                                 /**< Number of array entries. */
    uint8_t * p_data;                                               /**< Pointer to array entries. */
} uint8_array_t;

#ifdef __cplusplus
}
#endif

#undef PORTMACRO_INCLUDE
#endif // PORTMARCO_H
