/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Ambiq
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#include "dcd_apollo4.h"
#include "device/usbd.h"

#if TUSB_OPT_DEVICE_ENABLED && CFG_TUSB_MCU == OPT_MCU_APOLLO4

//!
//! This keeps track of init and power states for the USB module
//!
typedef struct
{
    bool isInited;
    bool isPoweredUp;
    bool vddPresent;

}
dcd_usb_state_t;

//!
//! This keeps track of init and power states for the USB module
//!
static dcd_usb_state_t dcd_usb_states = {
    .isInited = false,
    .isPoweredUp = false,
    .vddPresent = true,         ///< this is set to true to allow existing examples to run
};

static void *pUSBHandle = NULL;

static void dcd_usb_dev_evt_callback(am_hal_usb_dev_event_e eDevState);

static uint32_t dcd_usb_setHFRC2(bool bEnableClock);

static void dcd_usb_ep_xfer_complete_callback(const uint8_t ep_addr,
                                              const uint16_t xfer_len,
                                              am_hal_usb_xfer_code_e code,
                                              void *param);

static void dcd_usb_ep0_setup_callback(uint8_t *setup);


//*****************************************************************************
//! @brief  This is the event callback function
//!
//! @param eDevState
//*****************************************************************************
static void
dcd_usb_dev_evt_callback(am_hal_usb_dev_event_e eDevState)
{
    switch (eDevState)
    {
        case AM_HAL_USB_DEV_EVT_BUS_RESET:
            //
            // enable usb bus interrupts
            //
            am_hal_usb_intr_usb_enable(pUSBHandle, USB_CFG2_SOFE_Msk | USB_CFG2_ResumeE_Msk | USB_CFG2_SuspendE_Msk |
                                                   USB_CFG2_ResetE_Msk);
            //
            // init the endpoint
            //
            am_hal_usb_ep_init(pUSBHandle, 0, 0, 64);
#if BOARD_DEVICE_RHPORT_SPEED == OPT_MODE_FULL_SPEED
            am_hal_usb_set_dev_speed(pUSBHandle, AM_HAL_USB_SPEED_FULL);
            dcd_event_bus_reset(0, TUSB_SPEED_FULL, true);
#else
            dcd_event_bus_reset(0, TUSB_SPEED_HIGH, true);
#endif
            break;
        case AM_HAL_USB_DEV_EVT_RESUME:
            dcd_event_bus_signal(0, DCD_EVENT_RESUME, true);
            // Do something for resuming
            // then set the device state to active
            am_hal_usb_set_dev_state(pUSBHandle, AM_HAL_USB_DEV_STATE_ACTIVE);
            break;
        case AM_HAL_USB_DEV_EVT_SOF:
            dcd_event_bus_signal(0, DCD_EVENT_SOF, true);
            break;
        case AM_HAL_USB_DEV_EVT_SUSPEND:
            dcd_event_bus_signal(0, DCD_EVENT_SUSPEND, true);
            // Do something for suspending
            // then set the device state to suspended
            am_hal_usb_set_dev_state(pUSBHandle, AM_HAL_USB_DEV_STATE_SUSPENDED);
            break;
        default:
            // Not reachable case
            // add to suppress the compiling warning
            break;
    }
}

//*****************************************************************************
//! @brief  Setup request is received and pass it to upper layer TinyUSB
//!         stack to handle
//!
//! @param setup
//*****************************************************************************
static void
dcd_usb_ep0_setup_callback(uint8_t *setup)
{
    dcd_event_setup_received(0, setup, true);
}

//*****************************************************************************
//! @brief
//!
//! @param ep_addr
//! @param xfer_len
//! @param code
//! @param param
//*****************************************************************************
static void
dcd_usb_ep_xfer_complete_callback(const uint8_t ep_addr,
                                  const uint16_t xfer_len,
                                  am_hal_usb_xfer_code_e code,
                                  void *param)
{
    switch (code)
    {
        case USB_XFER_DONE:
            dcd_event_xfer_complete(0, ep_addr, xfer_len, XFER_RESULT_SUCCESS, true);
            break;
        case USB_XFER_STALL:
            dcd_event_xfer_complete(0, ep_addr, xfer_len, XFER_RESULT_STALLED, true);
            break;
        case USB_XFER_ABORT:
            break;
        case USB_XFER_DATA:
        case USB_XFER_UNSTALL:
        case USB_XFER_RESET:
        case USB_XFER_ERROR:
        default:
            if (xfer_len)
            {
                dcd_event_xfer_complete(0, ep_addr, xfer_len, XFER_RESULT_FAILED, true);
            }
            break;
    }
}

#undef AM_USB_CHARGER_DETECT
#ifdef AM_USB_CHARGER_DETECT

//*****************************************************************************
//! @brief
//!
//! @details The hardware detects a voltage higher than 4V on the VBUS
//! (USB power supply pin).
//! The hardware should provide a mechanism to make sure the VBUS voltage
//! can be compared against 4.0V.
//! This function is called only after the VBUS is more than 4.0V.
//! For example, the VBUS may come as the output of a comparator
//! (via interrupt or polling)
//!
//! @return am_hal_usb_charger_type_e
//*****************************************************************************
am_hal_usb_charger_type_e
am_hal_usb_BC1_2_good_battery_detection(void)
{
    int c, i;

    am_hal_usb_hardware_reset();

    //
    // Check VBUS voltage(USB power supply) is more than 4Volt.
    //
    am_util_delay_ms(10);

    am_hal_usb_charger_enable_data_pin_contact_detection();

    //
    // Check the data pin contact detection (DCD) for 900msec
    //
    c = 90;
    i = 2;
    while(c > 0)
    {
        //
        // Upon detecting VBUS greater than its VOTG_SESS_VLD threshold, a PD shall start a timer with a timeout
        // value of TDCD_TIMEOUT. A PD that supports DCD is allowed to enable its IDP_SRC and monitor for Dp being
        // at VLGC_LOW for TDCD_DBNC.
        // If the DCD timer expires before the Dp or ID conditions are detected, the PD shall proceed to Primary Detection.
        //
        c--;
        am_util_delay_ms(10);
        if( am_hal_usb_charger_data_pin_connection_status() == AM_HAL_USB_CHARGER_DATA_PIN_CONNECTED)
        {
            i--;

            //
            // Debouncing for data pins(Dm and Dp);T_DCD_DBNC(10msec) Data Contact Detect debounce
            //
            if(i == 0)
                break;
        }
        else
        {
            i = 2;
        }
    }

    //
    // Primary detection for good battery:
    //
    am_hal_usb_charger_enable_primary_detection();

    //
    // Start a timer for 100usec
    //
    c = 10;
    i = 3;
    am_util_delay_us(100);
    while(c > 0)
    {
        c--;
        am_util_delay_us(10);
        if (am_hal_usb_charger_sdp_connection_status() == AM_HAL_USB_CHARGER_SDP)
        {
            i--;
            if (i == 0)
            {
                //
                // pull-up the Dp pin to keep the current to 100mA(else the current may be reduced to 2.5mA)
                // or immidiately starts enumeration.
                // else the current can only be 2.5mA
                // "The SDP is detected"
                //
                return AM_HAL_USB_CHARGER_SDP;
            }
        }
    }

    //
    // Secondary detection for good battery:
    // If a PD Detects that it is attached to either a DCP or CDP during Primary Detection, and it is ready to be enumerated,
    // then it is allowed to take the branch where it connects. If a PD is not ready to be enumerated, then it is required
    // to do Secondary Detection.
    //
    am_hal_usb_charger_enable_secondary_detection();

    //
    // Start a timer for 100usec for analog circuit to settle down
    //
    am_util_delay_us(100);
    if (am_hal_usb_charger_cdp_or_dcp_connection_status() == AM_HAL_USB_CHARGER_CDP)
    {
        //
        // If a PD detects that Dp is less than VDAT_REF, it knows that it is attached to a CDP.
        // It is then required to turn off VDP_SRC and VDM_SRC, as shown in the Good Battery
        // Algorithm in Section 3.3.2, and is allowed to draw IDEV_CHG.
        //
        am_hal_usb_charger_enable_cdp_charger();

        //
        // "the CDP is detected"
        //
        return AM_HAL_USB_CHARGER_CDP;
    }

    if (am_hal_usb_charger_cdp_or_dcp_connection_status() == AM_HAL_USB_CHARGER_DCP)
    {
        //
        // If a PD detects that Dp is greater than VDAT_REF, it knows that it is attached to a DCP.
        // It is then required to enable VDP_SRC or pull Dp to VDP_UP through RDP_UP, as defined in
        // the Good Battery Algorithm in Section 3.3.2.
        //
        am_hal_usb_charger_enable_dcp_charger();
        // "the DCP is detected".
        return AM_HAL_USB_CHARGER_DCP;
    }

    return AM_HAL_USB_CHARGER_NO_CHARGER;
}

//*****************************************************************************
//! @brief
//!
//! @param bValid
//! @return am_hal_usb_charger_type_e
//*****************************************************************************
am_hal_usb_charger_type_e
dcd_usb_vbus_session(uin8_t rhport, bool bValid)
{
    (void) rhport;

    am_hal_usb_charger_type_e eChgType = AM_HAL_USB_CHARGER_NO_CHARGER;
    if (bValid == true)
    {
        //
        // VBUS session is valid
        //
        am_hal_usb_power_control(pUSBHandle, AM_HAL_SYSCTRL_WAKE, false);

        am_hal_usb_enable_phy_reset_override();

        eChgType = am_hal_usb_BC1_2_good_battery_detection();

        am_hal_usb_hardware_unreset();

        am_hal_usb_disable_phy_reset_override();

        if (eChgType == AM_HAL_USB_CHARGER_SDP)
        {

            //
            // Attach to USB host to start the enumeration
            //
            am_hal_usb_intr_usb_enable(pUSBHandle, USB_INTRUSB_Reset_Msk);
            dcd_connect(rhport);
        }
        else
        {
            //
            // Power off the USB peripheral for only power charging case
            //
            am_hal_usb_power_control(pUSBHandle, AM_HAL_SYSCTRL_NORMALSLEEP, false);
        }
    }
    else
    {
        //
        // VBUS session is invalid
        //
        dcd_disconnect();
        am_hal_usb_power_control(pUSBHandle, AM_HAL_SYSCTRL_NORMALSLEEP, false);
    }

    return eChgType;
}

static am_hal_usb_charger_type_e dcd_usb_charger_type;
#endif

/*------------------------------------------------------------------*/

//*****************************************************************************
//
//! @brief setup and enable HFRC2 when usb mode is highspeed
//!
//! @param enable  true will enable HFRC2
//! @param enable  false will disable HFRC2
//
//*****************************************************************************
static uint32_t
dcd_usb_setHFRC2(bool bEnableClock)
{

    am_hal_usb_hs_clock_type am_hal_hfrc2_clock_type = AM_HAL_USB_HS_CLK_DISABLE ;
    if ( bEnableClock )
    {
#if BOARD_DEVICE_RHPORT_SPEED == OPT_MODE_HIGH_SPEED
        am_hal_hfrc2_clock_type = AM_HAL_USB_HS_CLK_HFRC2_ADJ;
#endif
    }
    return am_hal_usb_control(AM_HAL_CLKGEN_CONTROL_SET_HFRC2_TYPE, &am_hal_hfrc2_clock_type ) ;

}

/*------------------------------------------------------------------*/

#ifndef ENABLE_EXT_USB_PWR_RAILS

#if defined(apollo4b_blue_evb) || defined(apollo4b_evb) || defined(apollo4b_evb_disp_shield) || \
    defined(apollo4p_evb) || defined(apollo4p_blue_evb) || \
    defined(apollo4p_evb_disp_shield) || defined(apollo4p_evb_disp_shield_rev1) || \
    defined(apollo4p_evb_disp_shield_rev2) || defined(apollo4p_blue_kbr_evb) || defined(apollo4p_blue_kxr_evb)

#define ENABLE_EXT_USB_PWR_RAILS

#endif
#endif // ENABLE_EXT_USB_PWR_RAILS

//*****************************************************************************
// @brief  Controller API
//
// @param rhport
//*****************************************************************************
void
dcd_init(uint8_t rhport)
{
    (void) rhport;

#if TUSB_ADDED_FUNCTIONS
    if (!dcd_usb_states.vddPresent ) return;
#endif

    //
    // Powerup Sequence
    // see Apollo4 Errata ERR041: Induced D+ output pulse may cause
    // unintended disconnect.
    //

    //
    // this function call doesn't set any USB registers, it configures the driver
    //
    uint32_t initStat = am_hal_usb_initialize(0, (void *) &pUSBHandle);
    if (initStat != AM_HAL_STATUS_SUCCESS) return;

    dcd_usb_states.isInited = true;
    dcd_usb_states.isPoweredUp = true;
    //
    // Register the callback functions
    //
    am_hal_usb_register_dev_evt_callback(pUSBHandle, dcd_usb_dev_evt_callback);
    am_hal_usb_register_ep0_setup_received_callback(pUSBHandle, dcd_usb_ep0_setup_callback);
    am_hal_usb_register_ep_xfer_complete_callback(pUSBHandle, dcd_usb_ep_xfer_complete_callback);

#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P)

    //
    // Rev B power up sequence. ERR041
    //

    //
    // enable internal power rail
    //
    am_hal_usb_power_control(pUSBHandle, AM_HAL_SYSCTRL_WAKE, false);

    //
    // clear USB PHY reset in MCU control registers
    //
    am_hal_usb_enable_phy_reset_override();


#ifdef ENABLE_EXT_USB_PWR_RAILS

    //
    // Enable the USB power rails
    //
    am_bsp_external_vddusb33_switch(true);
    am_bsp_external_vddusb0p9_switch(true);
    am_util_delay_ms(50);
#endif // ENABLE_EXT_USB_PWR_RAILS


#ifdef AM_USB_CHARGER_DETECT
    //
    // Simulate an PMIC or GPIO VBUS session interrupt
    //
    dcd_usb_charger_type = dcd_usb_vbus_session(rhport, true);

#else // !AM_USB_CHARGER_DETECT

    //
    // disable BC detection voltage source
    //
    am_hal_usb_hardware_unreset();
    //
    // start HFRC2 if the USB will run at HIGH speed
    // set argument to true, as we want the clock enabled if
	// high speed is enabled
    //
    uint32_t ui32hfrc2_status = dcd_usb_setHFRC2(true);

    //
    // set USB PHY reset disable
    //
    am_hal_usb_disable_phy_reset_override();

#if BOARD_DEVICE_RHPORT_SPEED == OPT_MODE_FULL_SPEED
    am_hal_usb_dev_speed_e eUsbSpeed = AM_HAL_USB_SPEED_FULL;
#else
    am_hal_usb_dev_speed_e eUsbSpeed = AM_HAL_USB_SPEED_HIGH;
#endif

    if (ui32hfrc2_status == AM_HAL_STATUS_SUCCESS)
    {
        am_hal_usb_set_dev_speed(pUSBHandle, eUsbSpeed);
    }

    //
    // enable usb module interrupts
    //
    am_hal_usb_intr_usb_enable(pUSBHandle, USB_INTRUSB_Reset_Msk);

    //
    // this implements the soft connect
    //
    dcd_connect(rhport);
#endif // END AM_USB_CHARGER_DETECT

#elif defined(AM_PART_APOLLO4)  // defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P)

    //
    // this is ERR041 REV A power up sequence
    //
    am_hal_usb_power_control(pUSBHandle, AM_HAL_SYSCTRL_WAKE, false);
#ifdef ENABLE_EXT_USB_PWR_RAILS

    //
    // Enable the USB power rails
    //
    am_bsp_external_vddusb33_switch(true);
    am_bsp_external_vddusb0p9_switch(true);
    am_util_delay_ms(50);
#endif // ENABLE_EXT_USB_PWR_RAILS
    am_hal_usb_intr_usb_enable(pUSBHandle, USB_INTRUSB_Reset_Msk);
#if BOARD_DEVICE_RHPORT_SPEED == OPT_MODE_FULL_SPEED
    am_hal_usb_set_dev_speed(pUSBHandle, AM_HAL_USB_SPEED_FULL);
#endif
    dcd_connect(rhport);
#endif // elif defined(AM_PART_APOLLO4)  // defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P)

}

//*****************************************************************************
// @brief enable device interrupt
//
//*****************************************************************************
void
dcd_int_enable(uint8_t rhport)
{
    (void) rhport;
    NVIC_SetPriority(USB0_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(USB0_IRQn);
}

//*****************************************************************************
// @brief disable device interrupt
//
//*****************************************************************************
void
dcd_int_disable(uint8_t rhport)
{
    (void) rhport;
    NVIC_DisableIRQ(USB0_IRQn);
}

//*****************************************************************************
//
// @brief Receive Set Address request, mcu port must also include status IN response
//
//
//*****************************************************************************
void
dcd_set_address(uint8_t rhport, uint8_t dev_addr)
{
    // Response with status first before changing device address
    dcd_edpt_xfer(rhport, tu_edpt_addr(0, TUSB_DIR_IN), NULL, 0);
    am_hal_usb_set_addr(pUSBHandle, dev_addr);
    am_hal_usb_set_dev_state(pUSBHandle, AM_HAL_USB_DEV_STATE_ADDRESSED);
}

//*****************************************************************************
// @brief
//
//*****************************************************************************
void
dcd_set_config(uint8_t rhport, uint8_t config_num)
{
    (void) rhport;
    (void) config_num;
    am_hal_usb_set_dev_state(pUSBHandle, AM_HAL_USB_DEV_STATE_CONFIGED);
}

//*****************************************************************************
//
// @brief Wake up host
//
//*****************************************************************************
void
dcd_remote_wakeup(uint8_t rhport)
{
    (void) rhport;
    am_hal_usb_start_remote_wakeup(pUSBHandle);
}

/*------------------------------------------------------------------*/
/* DCD Endpoint port
 *------------------------------------------------------------------*/


//*****************************************************************************
// @brief Configure endpoint's registers according to descriptor
//
//*****************************************************************************
bool
dcd_edpt_open(uint8_t rhport, tusb_desc_endpoint_t const *desc_edpt)
{
    (void) rhport;

    return am_hal_usb_ep_init((void *) pUSBHandle,
                              desc_edpt->bEndpointAddress,
                              (uint8_t) (desc_edpt->bmAttributes.xfer),
                              (uint16_t) (desc_edpt->wMaxPacketSize)) == AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// @brief Close all non-control endpoints, cancel all pending transfers if any.
// @note Invoked when switching from a non-zero Configuration by SET_CONFIGURE
// therefore required for multiple configuration support.
//
//*****************************************************************************
void
dcd_edpt_close_all(uint8_t rhport)
{
    (void) rhport;
}

//*****************************************************************************
//
// @brief Close an endpoint.
//
//*****************************************************************************
void
dcd_edpt_close(uint8_t rhport, uint8_t ep_addr)
{
    (void) rhport;
    (void) ep_addr;
}

//*****************************************************************************
//
// @brief Submit a transfer, When complete dcd_event_xfer_complete()
// is invoked to notify the stack
//
//*****************************************************************************
bool
dcd_edpt_xfer(uint8_t rhport,
              uint8_t ep_addr,
              uint8_t *buffer,
              uint16_t total_bytes)
{
    (void) rhport;

    return am_hal_usb_ep_xfer(pUSBHandle,
                              ep_addr,
                              buffer,
                              total_bytes) == AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// @brief Stall endpoint, any queuing transfer should be removed from endpoint
//
//*****************************************************************************
void
dcd_edpt_stall(uint8_t rhport, uint8_t ep_addr)
{
    (void) rhport;

    am_hal_usb_ep_stall(pUSBHandle, ep_addr);
}

//*****************************************************************************
//
// @brief  clear stall, data toggle is also reset to DATA0
//
//*****************************************************************************
void
dcd_edpt_clear_stall(uint8_t rhport, uint8_t ep_addr)
{
    am_hal_usb_ep_clear_stall(pUSBHandle, ep_addr);
}


//*****************************************************************************
//
// the usb isr
//
//*****************************************************************************
void
am_usb_isr(void)
{
    uint32_t ui32IntStatus[3];
    am_hal_usb_intr_status_get(pUSBHandle,
                               &ui32IntStatus[0],
                               &ui32IntStatus[1],
                               &ui32IntStatus[2]);
    am_hal_usb_interrupt_service(pUSBHandle,
                                 ui32IntStatus[0],
                                 ui32IntStatus[1],
                                 ui32IntStatus[2]);
}

//
//! check if functions not defined in tinyusb dcd.h are going to be used
//! these functions add disconnect/reconnect functinality
//
#if TUSB_ADDED_FUNCTIONS

//*****************************************************************************
// This is called when VUSB changes state
// note: the primary use to set VUSB is so the init function will run
// on system startup
//*****************************************************************************
void
dvd_set_vddPresent(uint8_t rhport, bool vddIsPresent, bool takeAction)
{
    dcd_usb_states.vddPresent = vddIsPresent ;

    if ( takeAction)
    {
        //
        // impose changes immediately
        //
        if (vddIsPresent)
        {
            dcd_powerup(rhport, false);
        }
        else
        {
            dcd_deInit(rhport) ;
        }
    }
}

//*****************************************************************************
//
// shutdown and powerdown the usb
//
//*****************************************************************************
void
dcd_deInit(uint8_t rhport)
{
    (void) rhport;

    if ( !dcd_usb_states.isInited ) {
        am_util_stdio_printf("dcd_deInit second call\n");

        return ;
    }

    dcd_usb_states.isInited = false;


    //
    // detach from the host
    //
    dcd_disconnect(rhport);

    //
    // wait for any interrupts to happen and be processed
    //
    am_util_delay_ms(50);

    //
    // disable USB interrupts
    //
    am_hal_usb_intr_usb_disable(pUSBHandle,
                                (USB_INTRUSB_SOF_Msk |
                                 USB_INTRUSB_Reset_Msk |
                                 USB_INTRUSB_Resume_Msk |
                                 USB_INTRUSB_Suspend_Msk));

    //
    // disable high speed clock
    //
    am_hal_usb_dev_speed_e eUsbSpeed = AM_HAL_USB_SPEED_FULL;
    am_hal_usb_set_dev_speed(pUSBHandle, eUsbSpeed);

    //
    // disable HFRC2
    //
    dcd_usb_setHFRC2(false);

    // power things down
    dcd_powerdown(rhport, false);

    am_hal_usb_deinitialize(pUSBHandle);

    pUSBHandle = NULL;

}

//*****************************************************************************
// This is called for a graceful USB power-up
//*****************************************************************************
void
dcd_powerup(uint8_t rhport, bool force)
{
    if (dcd_usb_states.isInited)
    {
        if ( pUSBHandle == 0 ) while ( 1 ) ;
        if (!dcd_usb_states.isPoweredUp || force)
        {
            dcd_usb_states.isPoweredUp = true;

            am_util_stdio_printf("in usb pwr up\n");

            //
            // enable internal power rail
            //
            am_hal_usb_power_control(pUSBHandle, AM_HAL_SYSCTRL_WAKE, false);
            //
            // clear USB PHY reset in MCU control registers
            //
            am_hal_usb_enable_phy_reset_override();

#ifdef ENABLE_EXT_USB_PWR_RAILS

            //
            // Enable the USB power rails
            //
            am_bsp_external_vddusb33_switch(true);
            am_bsp_external_vddusb0p9_switch(true);
            am_util_delay_ms(50);
#endif // ENABLE_EXT_USB_PWR_RAILS

            //
            // set USB PHY reset disable
            //
            am_hal_usb_disable_phy_reset_override();

            dcd_connect(rhport);
        }
        else
        {
            am_util_stdio_printf("in usb pwr up, already up\n");

        }
    }
    else
    {
        am_util_stdio_printf("in usb pwr up do init\n");
        dcd_init(rhport);
    }
}

//*****************************************************************************
// This is called for a graceful USB powerdown
//*****************************************************************************
void
dcd_powerdown(uint8_t rhport, bool force)
{
    (void) rhport;

    if (dcd_usb_states.isPoweredUp || force)
    {
        am_util_stdio_printf("in usb pwr dwn\n");
        dcd_usb_states.isPoweredUp = false;

        am_hal_usb_enable_phy_reset_override();

#ifdef ENABLE_EXT_USB_PWR_RAILS

        //
        // Disable the USB power rails
        //
        am_bsp_external_vddusb33_switch(false);
        am_bsp_external_vddusb0p9_switch(false);
        am_util_delay_ms(50);
#endif // ENABLE_EXT_USB_PWR_RAILS

        am_hal_usb_power_control(pUSBHandle, AM_HAL_SYSCTRL_DEEPSLEEP, false);
    }
}

//*****************************************************************************
//! @brief called by tinyusb when a device is unmounted
//!
//!
//*****************************************************************************
void tud_umount_cb(void)
{
    am_util_stdio_printf("tud_umount_cb\n");

    dcd_deInit(0) ;
}

//*****************************************************************************
//
// When using usb powerup/power down calls, this is the replacement for
// calling tud_task(),
//
// this will take care of power up power down and also call tud_task()
//
//*****************************************************************************

#endif //TUSB_ADDED_FUNCTIONS

//*****************************************************************************
// @brief  Enable/Disable Start-of-frame interrupt. Default is disabled
//*****************************************************************************
void dcd_sof_enable(uint8_t rhport, bool en)
{
    (void) rhport;

    if(en)
    {
        am_hal_usb_enable_sof_intr(pUSBHandle);
    }
    else
    {
        am_hal_usb_disable_sof_intr(pUSBHandle);
    }
}

//*****************************************************************************
// @brief Connect by enabling internal pull-up resistor on D+/D-
//
//*****************************************************************************
void
dcd_connect(uint8_t rhport)
{
    (void) rhport;
    am_hal_usb_attach(pUSBHandle);
}

//*****************************************************************************
// @brief Disconnect by disabling internal pull-up resistor on D+/D-
//
//*****************************************************************************
void
dcd_disconnect(uint8_t rhport)
{
    (void) rhport;
    am_hal_usb_detach(pUSBHandle);
}

#endif // TUSB_OPT_DEVICE_ENABLED && CFG_TUSB_MCU == OPT_MCU_APOLLO4


