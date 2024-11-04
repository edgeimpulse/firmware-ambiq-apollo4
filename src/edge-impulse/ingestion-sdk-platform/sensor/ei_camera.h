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

#ifndef _EI_CAMERA_H_
#define _EI_CAMERA_H_

#include "firmware-sdk/ei_camera_interface.h"
#include "firmware-sdk/ei_device_info_lib.h"

#ifdef apollo510_evb
    #define CAM_SPI_IOM 2
#else
    #define CAM_SPI_IOM 1
#endif
#define CAM_SPI_SPEED AM_HAL_IOM_8MHZ

// Camera definitions
// Following includes camera specific header

#define RGB_WIDTH 160
#define RGB_HEIGHT 160
#define RGB_CH_SIZE (RGB_WIDTH * RGB_HEIGHT)

// This is RGB565, so 2 bytes per pixel
#define RGB_BUFF_SIZE (2 * RGB_CH_SIZE)

// This is RGB565, so 2 bytes per pixel
#define RGB888_BUFF_SIZE (3 * RGB_CH_SIZE)

#define JPG_WIDTH 320
#define JPG_HEIGHT 320
#define JPG_CH_SIZE (JPG_WIDTH * JPG_HEIGHT)
#define JPG_BUFF_SIZE (JPG_CH_SIZE)

class EiAmbiqCamera : public EiCamera
{
private:
    static ei_device_snapshot_resolutions_t resolutions[];
    bool camera_found;
    bool tried_init;
    uint16_t width;
    uint16_t height;
    uint8_t img_mode_index;
public:
    EiAmbiqCamera();
    bool is_camera_present(void) {return camera_found;};
    bool init(uint16_t width, uint16_t height) override;
    bool deinit(void) override; 
    void get_resolutions(ei_device_snapshot_resolutions_t **res, uint8_t *res_num) override;
    bool set_resolution(const ei_device_snapshot_resolutions_t res) override;
    ei_device_snapshot_resolutions_t get_min_resolution(void) override;
    ei_device_snapshot_resolutions_t search_resolution(uint32_t required_width, uint32_t required_height) override;

    bool ei_camera_capture_rgb888_packed_big_endian(
        uint8_t *image,
        uint32_t image_size) override;

    bool get_fb_ptr(uint8_t** fb_ptr) override;
};

#endif
