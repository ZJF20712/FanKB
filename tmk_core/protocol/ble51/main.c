#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "lufa.h"
#include "print.h"
#include "sendchar.h"
#include "ble51.h"
#include "ble51_task.h"
#include "serial.h"
#include "keyboard.h"
#include "keycode.h"
#include "action.h"
#include "action_util.h"
#include "wait.h"
#include "timer.h"
#include "suspend.h"

//extern bool force_bt;
//extern bool usb_connected;
bool kb_started = 0;

static int8_t sendchar_func(uint8_t c)
{
    //xmit(c);        // SUART
    sendchar(c);    // LUFA
    return 0;
}

static watchdog_on(void) {
    wdt_disable();
    wdt_enable(WDTO_500MS);
}

static void SetupHardware(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    //MCUSR &= ~(1 << WDRF);
    //wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    // Leonardo needs. Without this USB device is not recognized.
    USB_Disable();

    USB_Init();

#ifdef CONSOLE_ENABLE
    // for Console_Task
    USB_Device_EnableSOFEvents();
    print_set_sendchar(sendchar_func);
#endif
}

int main(void)  __attribute__ ((weak));
int main(void)
{
    SetupHardware();
    sei();

    /* wait for USB startup to get ready for debug output */
    uint8_t timeout = 255;  // timeout when USB is not available(Bluetooth)
    while (timeout-- && USB_DeviceState != DEVICE_STATE_Configured) {
        wait_ms(4);
#if defined(INTERRUPT_CONTROL_ENDPOINT)
        ;
#else
        USB_USBTask();
#endif
    }
    /*if (USB_DeviceState == DEVICE_STATE_Configured) {
        force_bt = true;
        usb_connected = true;
    }*/
//    print("\nUSB init\n");

    ble51_init();
    ble51_task_init();

    /* init modules */
    keyboard_init();

//    print("Keyboard start\n");
    kb_started = 1;
    watchdog_on();
    while (1) {
        if (BLE51_PowerState > 1) { 
            select_all_rows();
            suspend_power_down();
            if (suspend_wakeup_condition()) {
                unselect_rows();
                watchdog_on();
                suspend_wakeup_init();
                if (BLE51_PowerState >= 4) {
                    turn_on_bt();
                }
                BLE51_PowerState = 1;
            }
        }
        if (BLE51_PowerState < 4){
            wdt_reset();
            if (BLE51_PowerState <= 1) 
				keyboard_task();
            ble51_task();
        }
    }
}
