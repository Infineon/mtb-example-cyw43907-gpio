/******************************************************************************
* File Name:   main.c
*
* Description: This code example demonstrates the use of GPIO configured as an
*              input pin to generate interrupts in CYW43907 MCU.
*
* Related Document: README.md
*
*******************************************************************************
* $ Copyright 2021-2023 Cypress Semiconductor $
*******************************************************************************/

#include "cy_retarget_io.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_utils.h"

/******************************************************************************
 * Macros
 *****************************************************************************/
#define DELAY_SHORT_MS          (250)   /* milliseconds */
#define DELAY_LONG_MS           (500)   /* milliseconds */
#define GPIO_INTERRUPT_PRIORITY (3u)    /* unused currently*/

/*******************************************************************************
* Function Prototypes
********************************************************************************/
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event);

/*******************************************************************************
* Global Variables
********************************************************************************/
volatile bool gpio_intr_flag = false;

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*   System entrance point. This function configures and initializes the GPIO
*   interrupt, update the delay on every GPIO interrupt, blinks the LED.
*
*
* Return: int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    uint32_t delay_led_blink = DELAY_LONG_MS;

    cyhal_gpio_callback_data_t cb_data =
    {
        .callback     = gpio_interrupt_handler,
        .callback_arg = (void*)NULL
    };

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize retarget-io to use the debug UART port. */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                 CY_RETARGET_IO_BAUDRATE);

    /* UART init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the user LED */
    result = cyhal_gpio_init(CYBSP_LED2, CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    /* gpio init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the user button */
    result = cyhal_gpio_init(CYBSP_SW1, CYHAL_GPIO_DIR_INPUT,
                    CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);

    /* gpio init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Configure GPIO interrupt */
    cyhal_gpio_register_callback(CYBSP_SW1,
            &cb_data);
    cyhal_gpio_enable_event(CYBSP_SW1, CYHAL_GPIO_IRQ_FALL,
                                 GPIO_INTERRUPT_PRIORITY, true);

    /* Enable global interrupts */
   __enable_irq();

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");
    printf("**************** CYW43907 MCU: GPIO Interrupt *****************\r\n");

    for (;;)
    {
        /* Check the interrupt status */
        if (true == gpio_intr_flag)
        {
            gpio_intr_flag = false;

            /* Update LED toggle delay */
            delay_led_blink = (DELAY_LONG_MS == delay_led_blink) ? DELAY_SHORT_MS : DELAY_LONG_MS;
        }

        cyhal_gpio_write(CYBSP_LED2, CYBSP_LED_STATE_ON);
        cyhal_system_delay_ms(delay_led_blink);
        cyhal_gpio_write(CYBSP_LED2, CYBSP_LED_STATE_OFF);
        cyhal_system_delay_ms(delay_led_blink);

    }
}

/*******************************************************************************
* Function Name: gpio_interrupt_handler
********************************************************************************
* Summary:
*   GPIO interrupt handler updates the gpio_intr_flag.
*   
* Parameters:
*  void *handler_arg (unused)
*  cyhal_gpio_irq_event_t (unused)
*
*******************************************************************************/
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    CY_UNUSED_PARAMETER(handler_arg);
    CY_UNUSED_PARAMETER(event);
    gpio_intr_flag = true;
}

/* [] END OF FILE */
