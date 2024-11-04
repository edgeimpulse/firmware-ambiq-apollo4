/*
 * Copyright (c) 2024 EdgeImpulse Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "FreeRTOS.h"
#include "task.h"

#include "peripheral/peripheral.h"
#include "peripheral/usb/ei_usb.h"
#include "common_events.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "edge-impulse/ingestion-sdk-platform/apollo4/ei_device_apollo4.h"
#include "edge-impulse/ingestion-sdk-platform/apollo4/ei_at_handlers.h"
#include "inference/ei_run_impulse.h"
#include "edge-impulse/ingestion-sdk-platform/sensor/ei_mic.h"

#if (configAPPLICATION_ALLOCATED_HEAP == 1)
size_t ucHeapSize = (NS_MALLOC_HEAP_SIZE_IN_K + 4) * 1024;
uint8_t ucHeap[(NS_MALLOC_HEAP_SIZE_IN_K + 4) * 1024] __attribute__((aligned(4)));
#endif

// main task parameters
#define EI_MAIN_TASK_STACK_SIZE_BYTE        (4096u)
#define EI_MAIN_TASK_PRIORITY               (configMAX_PRIORITIES - 2)
static TaskHandle_t ei_main_task_handle;
static void ei_main_task(void *pvParameters);

EventGroupHandle_t common_event_group;

/**
 * @brief Main
 *
 * @return int
 */
int main(void)
{
    peripheral_init();  // init basic peripheral, core etc
    ei_usb_init();         // init usb
    ei_microphone_init();
        
    /* create a task to send data via usb */
    if (xTaskCreate(ei_main_task,
        (const char*) "EI Main Thread",
        EI_MAIN_TASK_STACK_SIZE_BYTE / 4, // in words
        NULL, //pvParameters
        EI_MAIN_TASK_PRIORITY, //uxPriority
        &ei_main_task_handle) != pdPASS) {
        ei_printf("Failed to create EI Main Thread\r\n");
        
        while(1);
    }

    common_event_group = xEventGroupCreate();

    vTaskStartScheduler();
    while (1) {
    };
}

/**
 * @brief 
 * 
 * @param pvParameters 
 */
static void ei_main_task(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t event_bit;
    bool in_rx_loop = false;

    EiAmbiqApollo4 *dev = static_cast<EiAmbiqApollo4*>(EiDeviceInfo::get_device());
    ATServer *at;
    
    ei_printf("Edge Impulse SDK - Ambiq Apollo 4");
    ei_printf("Type AT+HELP to see a list of scommands.\r\n");
    ei_printf("Starting main loop\r\n");

    at = ei_at_init(dev);
    at->print_prompt();
    dev->get_camera()->init(160, 160);

        // Event loop
    while (1) {

        event_bit = xEventGroupWaitBits(common_event_group, 
                                        EVENT_RX_READY,    //  uxBitsToWaitFor 
                                        pdTRUE,                 //  xClearOnExit
                                        pdFALSE,                //  xWaitForAllBits
                                        portMAX_DELAY);

        if (event_bit & EVENT_RX_READY) {
            char data = ei_get_serial_byte(is_inference_running());

            in_rx_loop = false;

            while ((uint8_t)data != 0xFF) {
                if ((is_inference_running() == true) && (data == 'b') && (in_rx_loop == false)) {
                    ei_stop_impulse();
                    at->print_prompt();
                    continue;
                }

                in_rx_loop = true;
                at->handle(data);
                data = ei_get_serial_byte(is_inference_running());
            }
        }


    } // while(1)
}
