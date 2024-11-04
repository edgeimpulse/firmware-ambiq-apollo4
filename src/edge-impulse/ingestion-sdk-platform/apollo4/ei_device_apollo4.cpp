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
/* Includes ---------------------------------------------------------------- */
#include "ei_device_apollo4.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "edge-impulse-sdk/dsp/ei_utils.h"
#include "ingestion-sdk-platform/sensor/ei_mic.h"
#include "hal/am_hal_global.h"

EiAmbiqApollo4::EiAmbiqApollo4(EiDeviceMemory* mem)
{
    EiDeviceInfo::memory = mem;

    init_device_id();
    load_config();

    device_type = "AMBIQ_APOLLO4";

    /* Init standalone sensor */
    sensors[ANALOG_MICROPHONE].name = "Analog Microphone";
    sensors[ANALOG_MICROPHONE].frequencies[0] = 16000;
    sensors[ANALOG_MICROPHONE].max_sample_length_s = 5;
    sensors[ANALOG_MICROPHONE].start_sampling_cb = ei_analog_microphone_sample_start_mono;

    sensors[PDM_MICROPHONE].name = "PDM Microphone";
    sensors[PDM_MICROPHONE].frequencies[0] = 16000;
    sensors[PDM_MICROPHONE].max_sample_length_s = 5;
    sensors[PDM_MICROPHONE].start_sampling_cb = ei_pdm_microphone_sample_start_mono;
}

/**
 * @brief 
 * 
 */
void EiAmbiqApollo4::init_device_id(void)
{
    char temp[20] = {"12345"};

    device_id = std::string(temp);

    /* Init camera instance */
    camera = static_cast<EiAmbiqCamera*>(EiCamera::get_camera());
}

bool EiAmbiqApollo4::get_sensor_list(const ei_device_sensor_t **p_sensor_list, size_t *sensor_list_size)
{
    *p_sensor_list = sensors;
    *sensor_list_size = ARRAY_LENGTH(sensors);
    return true;
}

/**
 * @brief      Create resolution list for snapshot setting
 *             The studio and daemon require this list
 * @param      snapshot_list       Place pointer to resolution list
 * @param      snapshot_list_size  Write number of resolutions here
 *
 * @return     False if all went ok
 */
EiSnapshotProperties EiAmbiqApollo4::get_snapshot_list(void)
{
    ei_device_snapshot_resolutions_t *res = NULL;

    uint8_t res_num = 0;
    EiSnapshotProperties props = {
        .has_snapshot = false,
        .support_stream = false,
        .color_depth = "",
        .resolutions_num = res_num,
        .resolutions = res
    };

    if (camera->is_camera_present() == true) {
        this->camera->get_resolutions(&res, &res_num);
        props.has_snapshot = true;
        props.support_stream = true;        
        props.color_depth = "RGB";
        props.resolutions_num = res_num;
        props.resolutions = res;

        return props; 
    }

    camera->get_resolutions(&res, &res_num);

    return props;
}

/**
 * @brief get_device is a static method of EiDeviceInfo class
 * It is used to implement singleton paradigm, so we are returning
 * here pointer always to the same object (dev)
 * 
 * @return EiDeviceInfo* 
 */
EiDeviceInfo* EiDeviceInfo::get_device(void)
{
    AM_SHARED_RW static EiDeviceRAM<131072, 4> memory(sizeof(EiConfig));
    static EiAmbiqApollo4 dev(&memory);

    return &dev;
}
