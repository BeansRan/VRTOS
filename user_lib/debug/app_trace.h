#ifndef __APP_TRACE_H__
#define __APP_TRACE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

/* Function declaration ----------------------------------------------------- */
void app_trace_dump(uint8_t *str, uint32_t len);
uint32_t app_get_heap_size(void);

/* User definition ---------------------------------------------------------- */
#define USE_PLATFORM_KEIL					0x01
#define USE_PLATFORM_IAR					0x02

//#define __BUILD_PLATFORM					USE_PLATFORM_KEIL
//#define __BUILD_PLATFORM					USE_PLATFORM_IAR

#define SYS_REMAIN_HEAP_SIZE		        app_get_heap_size()														

#define ENABLE_DEBUG_TRACE_LOG		        (1)
#if (1 == ENABLE_DEBUG_TRACE_LOG)
#define app_trace_log                       printf
#else
#define app_trace_log(...)
#endif

#define TRACE_LEVEL                         (2)
#if(1 == TRACE_LEVEL)
#define LOG(fmt, ...)       		        app_trace_log("[%5d][%4d]"fmt, SYS_REMAIN_HEAP_SIZE, __LINE__, ##__VA_ARGS__)     
#define LOG_ERROR(fmt, ...)       		    app_trace_log("[%5d][%4d][error]"fmt, SYS_REMAIN_HEAP_SIZE, __LINE__, ##__VA_ARGS__)  
#define LOG_DUMP                            app_trace_dump
#elif(2 == TRACE_LEVEL)
  #define LOG(fmt, ...)                     app_trace_log("[%5d]"fmt, __LINE__, ##__VA_ARGS__)
  #define LOG_ERROR(fmt, ...)               app_trace_log("[%5d][error]"fmt, __LINE__, ##__VA_ARGS__)
  #define LOG_DUMP                          app_trace_dump
#else
  #define LOG            			        
  #define LOG_ERROR
  #define LOG_DUMP                          
#endif

#endif



