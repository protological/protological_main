

#include <stdio.h>
#include <stdlib.h>

#include "diag/Trace.h"

#include "Timer.h"

/* wolfSSL */
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>



// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (TIMER_FREQUENCY_HZ * 2 / 4)
#define BLINK_OFF_TICKS (TIMER_FREQUENCY_HZ - BLINK_ON_TICKS)


// Port numbers: 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G, ...
#if 1
#define BLINK_PORT_NUMBER               (0)
#define BLINK_PIN_NUMBER                (9)
#define BLINK_ACTIVE_LOW                (0)
#else
#define BLINK_PORT_NUMBER               (0)
#define BLINK_PIN_NUMBER                (10)
#define BLINK_ACTIVE_LOW                (1)
#endif

#define BLINK_GPIOx(_N)                 ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define BLINK_PIN_MASK(_N)              (1 << (_N))
#define BLINK_RCC_MASKx(_N)             (RCC_APB2Periph_GPIOA << (_N))

void blink_led_init()
{
  // Enable GPIO Peripheral clock
  RCC_APB2PeriphClockCmd(BLINK_RCC_MASKx(BLINK_PORT_NUMBER), ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in output push/pull mode
  GPIO_InitStructure.GPIO_Pin = BLINK_PIN_MASK(BLINK_PIN_NUMBER);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(BLINK_GPIOx(BLINK_PORT_NUMBER), &GPIO_InitStructure);

  // Start with led turned off
  GPIO_ResetBits(BLINK_GPIOx(BLINK_PORT_NUMBER),BLINK_PIN_MASK(BLINK_PIN_NUMBER));
}


int main()
{
    // Send a greeting to the trace device (skipped on Release).
    trace_puts("Hello ARM World!");

    // At this stage the system clock should have already been configured
    // at high speed.
    trace_printf("System clock: %u Hz\n", SystemCoreClock);

    timer_start();

    blink_led_init();

    {
        /* declare wolfSSL objects */
        WOLFSSL_CTX* ctx;
        WOLFSSL*     ssl;


        /* Initialize wolfSSL */
        wolfSSL_Init();

        ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());

    }

    uint32_t seconds = 0;

    // Infinite loop
    while (1)
    {
        GPIO_SetBits(BLINK_GPIOx(BLINK_PORT_NUMBER),BLINK_PIN_MASK(BLINK_PIN_NUMBER));
        timer_sleep(seconds == 0 ? TIMER_FREQUENCY_HZ : BLINK_ON_TICKS);

        GPIO_ResetBits(BLINK_GPIOx(BLINK_PORT_NUMBER),BLINK_PIN_MASK(BLINK_PIN_NUMBER));
        timer_sleep(BLINK_OFF_TICKS);

        ++seconds;

        // Count seconds on the trace device.
        trace_printf("Second %u\n", seconds);
    }

    // return
}


// EOF
