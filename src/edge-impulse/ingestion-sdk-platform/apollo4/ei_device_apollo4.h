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

/* Include ----------------------------------------------------------------- */
#ifndef _EI_DEVICE_APOLLO4_H_
#define _EI_DEVICE_APOLLO4_H_

#include "firmware-sdk/ei_device_info_lib.h"
#include "firmware-sdk/ei_device_memory.h"
#include "ingestion-sdk-platform/sensor/ei_camera.h"

#define MAX_BAUD        (36846400u)

/** Sensors */
typedef enum
{
    ANALOG_MICROPHONE      = 0,
    PDM_MICROPHONE         = 1,
    e_last_sensor
}used_sensors_t;

/** Number of sensors used */
#define EI_DEVICE_N_SENSORS            (e_last_sensor)

class EiAmbiqApollo4 : public EiDeviceInfo {
private:
    ei_device_sensor_t sensors[EI_DEVICE_N_SENSORS];
    EiAmbiqCamera *camera;

public:
    EiAmbiqApollo4(EiDeviceMemory* mem);
    ~EiAmbiqApollo4() {};

    void init_device_id(void) override;
    bool get_sensor_list(const ei_device_sensor_t **p_sensor_list, size_t *sensor_list_size) override;    
    EiSnapshotProperties get_snapshot_list(void) override;
    EiAmbiqCamera* get_camera(void) { return camera; };
};

#endif