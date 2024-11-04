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
#include "inference_task.h"
#include "inference/ei_run_impulse.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"

// Inference task parameters
#define INFRENCE_TASK_STACK_SIZE_BYTE        (4096u)
#define INFRENCE_TASK_PRIORITY               (configMAX_PRIORITIES - 3)
static TaskHandle_t inference_task_handle = NULL;
static void inference_task(void *pvParameters);

void inference_task_start(void)
{
    /* Only init once */
    if (inference_task_handle != NULL) {
        return;
    }

    if (xTaskCreate(inference_task,
        (const char*) "Inference task",
        INFRENCE_TASK_STACK_SIZE_BYTE / 4, // in words
        NULL, //pvParameters
        INFRENCE_TASK_PRIORITY, //uxPriority
        &inference_task_handle) != pdPASS) {
        ei_printf("Failed to create Display task\r\n");
    }
}

/**
 * @brief 
 * 
 * @param pvParameters 
 */
void inference_task(void *pvParameters)
{
    (void)pvParameters;

    while(1) {
        while(is_inference_running() == true) {
            ei_run_impulse();
        }
    }

    /* Unblock other tasks */
    vTaskDelay(10);
}
