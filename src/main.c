#include "xparameters.h"
#include "xgpio.h"
#include "xil_printf.h"

/* * CRITICAL FIX: 
 * Your xparameters.h shows only ONE GPIO instance (AXI_GPIO_0).
 * It is a "Dual Channel" device.
 * Channel 1 = LEDs (8 bits)
 * Channel 2 = Switches
 */

// If DEVICE_ID isn't found, we use '0' because you only have one GPIO block.
#ifndef XPAR_AXI_GPIO_0_DEVICE_ID
#define XPAR_AXI_GPIO_0_DEVICE_ID 0
#endif

#define GPIO_DEVICE_ID  XPAR_AXI_GPIO_0_DEVICE_ID

#define LED_CHANNEL 2
#define SW_CHANNEL  1

XGpio GpioInst; // ONE driver instance for BOTH channels

int main()
{
    int status;
    u32 read_sw;
    volatile int delay_count;
    int period;
    int i;

    xil_printf("--- Knight Rider Chaser Started (Dual Channel Mode) ---\n\r");

    /* --- STEP 1: INITIALIZE DRIVER (Once!) --- */
    status = XGpio_Initialize(&GpioInst, GPIO_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("GPIO Initialization Failed\r\n");
        return XST_FAILURE;
    }

    /* --- STEP 2: SET DIRECTIONS --- */
    // Channel 1 (LEDs) = Output (0x00)
    XGpio_SetDataDirection(&GpioInst, LED_CHANNEL, 0x00);
    
    // Channel 2 (Switches) = Input (0xFF)
    XGpio_SetDataDirection(&GpioInst, SW_CHANNEL, 0xFF);

    /* --- STEP 3: THE LOOP --- */
    while (1) {
        
        // --- PART 1: MOVE RIGHT (LD0 -> LD7) ---
        for (i = 0; i < 8; i++) {
            
            // Read Channel 2 (Switches)
            // Mask with 0x01 because we only care about SW0
            read_sw = XGpio_DiscreteRead(&GpioInst, SW_CHANNEL) & 0x01;
            
            // Logic: SW0 UP (1) = FAST
            if (read_sw != 0) 
                period = 6000000;
            else 
                period = 25000000;

            // Write to Channel 1 (LEDs)
            XGpio_DiscreteWrite(&GpioInst, LED_CHANNEL, (1 << i));

            for (delay_count = 0; delay_count < period; delay_count++);
        }

        // --- PART 2: MOVE LEFT (LD6 -> LD1) ---
        for (i = 6; i > 0; i--) {
            
            read_sw = XGpio_DiscreteRead(&GpioInst, SW_CHANNEL) & 0x01;
            
            if (read_sw != 0) 
                period = 6000000;
            else 
                period = 25000000;

            XGpio_DiscreteWrite(&GpioInst, LED_CHANNEL, (1 << i));

            for (delay_count = 0; delay_count < period; delay_count++);
        }
    }

    return 0;
}