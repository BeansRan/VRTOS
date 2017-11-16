
#include "app_trace.h"
#include "usart.h"

#define DBG_UART_HANDLER                    &huart2
#define __BUILD_PLATFORM  0

#if ( __BUILD_PLATFORM == USE_PLATFORM_IAR )
#include <iar_dlmalloc.h>
#endif

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#if 1
//#pragma import(__use_no_semihosting_swi)
struct __FILE { int handle;} ;

FILE __stdout;
FILE __stdin;
FILE __stderr;

int fgetc(FILE *f) {
  return 0;
}

void _sys_exit(int return_code){
  label:goto label;
}

void _ttywrch(int ch) {
  HAL_UART_Transmit(DBG_UART_HANDLER, (uint8_t *)&ch, 1, 200);
}

int _ferror(FILE *f){
  return EOF;
}
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(DBG_UART_HANDLER, (uint8_t *)&ch, 1, 200);

  return ch;
}


uint32_t app_get_heap_size(void) {
    uint32_t heap_size = 0;
  
#if ( __BUILD_PLATFORM == USE_PLATFORM_IAR )
	struct mallinfo m;
	m = __iar_dlmallinfo();
	heap_size = m.fordblks;
#elif ( __BUILD_PLATFORM == USE_PLATFORM_KEIL )
	char stat[60] = {0x00};
	__heapstats((__heapprt)sprintf, stat);
	char *size = stat + (strlen(stat)+2);
	size = strstr(size, "size ");
	sscanf(size, "size %u", &heap_size);
#endif
	return heap_size;
}


void app_trace_dump(uint8_t *str, uint32_t len) {
  for(uint32_t i = 0; i < len; i++){
    app_trace_log("%02X ", *str++);
  }
  app_trace_log("\r\n");
}

