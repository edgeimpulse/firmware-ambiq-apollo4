//*****************************************************************************
//
//! @file am_hal_clkgen.h
//!
//! @brief Functions for interfacing with the CLKGEN.
//!
//! @addtogroup clkgen4_4l CLKGEN - Clock Generator
//! @ingroup apollo4l_hal
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2024, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk_4_5_0-a1ef3b89f9 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AM_HAL_CLKGEN_H
#define AM_HAL_CLKGEN_H

#include "am_hal_mcuctrl.h"

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! @name System Clock max frequency
//! @{
//! Defines the maximum clock frequency for this device.
//! These macros provide a definition of the maximum clock frequency.
//
//*****************************************************************************
#ifdef APOLLO4_FPGA
#define AM_HAL_CLKGEN_FREQ_MAX_HZ           (APOLLO4_FPGA * 1000000)
#else // APOLLO4_FPGA
#define AM_HAL_CLKGEN_FREQ_MAX_HZ           96000000
#endif // APOLLO4_FPGA

#define AM_HAL_CLKGEN_FREQ_MAX_KHZ      (AM_HAL_CLKGEN_FREQ_MAX_HZ / 1000)
#define AM_HAL_CLKGEN_FREQ_MAX_MHZ      (AM_HAL_CLKGEN_FREQ_MAX_HZ / 1000000)
#define AM_HAL_CLKGEN_CORESEL_MAXDIV    1
//! @}

//
//! Control operations.
//
typedef enum
{
    AM_HAL_CLKGEN_CONTROL_RTC_SEL_XTAL,
    AM_HAL_CLKGEN_CONTROL_RTC_SEL_LFRC,
    AM_HAL_CLKGEN_CONTROL_HFADJ_ENABLE,
    AM_HAL_CLKGEN_CONTROL_HFADJ_DISABLE,
    AM_HAL_CLKGEN_CONTROL_HF2ADJ_ENABLE,
    AM_HAL_CLKGEN_CONTROL_HF2ADJ_DISABLE,
    AM_HAL_CLKGEN_CONTROL_HF2ADJ_COMPUTE,
    AM_HAL_CLKGEN_CONTROL_HFRC2_START,
    AM_HAL_CLKGEN_CONTROL_HFRC2_STOP,
    AM_HAL_CLKGEN_CONTROL_RESERVED1,
    AM_HAL_CLKGEN_CONTROL_RESERVED2,
    AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_OFF,
    AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_HFRC48,
    AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_HFRC96,
    AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_DPHYPLL,
    AM_HAL_CLKGEN_CONTROL_PLLCLK_ENABLE,
    AM_HAL_CLKGEN_CONTROL_PLLCLK_DISABLE,
    AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_OFF,
    AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_HFRC12,
    AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_HFRC6,
    AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_HFXT
}
am_hal_clkgen_control_e;

//
//! Current RTC oscillator.
//
typedef enum
{
    AM_HAL_CLKGEN_STATUS_RTCOSC_XTAL,
    AM_HAL_CLKGEN_STATUS_RTCOSC_LFRC,
}
am_hal_clkgen_status_rtcosc_e;

//
//! Clock Generation CLKOUT
//
typedef enum
{
    AM_HAL_CLKGEN_CLKOUT_XTAL_32768     = CLKGEN_CLKOUT_CKSEL_XT,           // XTAL
    AM_HAL_CLKGEN_CLKOUT_XTAL_16384     = CLKGEN_CLKOUT_CKSEL_XT_DIV2,      // XTAL / 2
    AM_HAL_CLKGEN_CLKOUT_XTAL_8192      = CLKGEN_CLKOUT_CKSEL_XT_DIV4,      // XTAL / 4
    AM_HAL_CLKGEN_CLKOUT_XTAL_4096      = CLKGEN_CLKOUT_CKSEL_XT_DIV8,      // XTAL / 8
    AM_HAL_CLKGEN_CLKOUT_XTAL_2048      = CLKGEN_CLKOUT_CKSEL_XT_DIV16,     // XTAL / 16
    AM_HAL_CLKGEN_CLKOUT_XTAL_1024      = CLKGEN_CLKOUT_CKSEL_XT_DIV32,     // XTAL / 32
    AM_HAL_CLKGEN_CLKOUT_XTAL_128       = CLKGEN_CLKOUT_CKSEL_XT_DIV256,    // XTAL / 256   = 128 Hz
    AM_HAL_CLKGEN_CLKOUT_XTAL_4         = CLKGEN_CLKOUT_CKSEL_XT_DIV8K,     // XTAL / 8192  =  4 Hz
    AM_HAL_CLKGEN_CLKOUT_XTAL_0_5       = CLKGEN_CLKOUT_CKSEL_XT_DIV64K,    // XTAL / 65536 =  0.5 Hz
    AM_HAL_CLKGEN_CLKOUT_XTAL_0_015     = CLKGEN_CLKOUT_CKSEL_XT_DIV2M,     // XTAL / 2097152 = 0.015625 Hz

    AM_HAL_CLKGEN_CLKOUT_LFRC           = CLKGEN_CLKOUT_CKSEL_LFRC,         // LFRC
    AM_HAL_CLKGEN_CLKOUT_LFRC_512       = CLKGEN_CLKOUT_CKSEL_LFRC_DIV2,    // LFRC / 2     = 512 Hz
    AM_HAL_CLKGEN_CLKOUT_LFRC_32        = CLKGEN_CLKOUT_CKSEL_LFRC_DIV32,   // LFRC / 32    =  32 Hz
    AM_HAL_CLKGEN_CLKOUT_LFRC_2         = CLKGEN_CLKOUT_CKSEL_LFRC_DIV512,  // LFRC / 512   =   2 Hz
    AM_HAL_CLKGEN_CLKOUT_LFRC_0_03      = CLKGEN_CLKOUT_CKSEL_LFRC_DIV32K,  // LFRC / 32768 = 0.03125 Hz
    AM_HAL_CLKGEN_CLKOUT_LFRC_0_0010    = CLKGEN_CLKOUT_CKSEL_LFRC_DIV1M,   // LFRC / 1M    = 0.0009765625 Hz

    AM_HAL_CLKGEN_CLKOUT_HFRC_48M       = CLKGEN_CLKOUT_CKSEL_HFRC_DIV2,    // HFRC / 2      = 48MHz
    AM_HAL_CLKGEN_CLKOUT_HFRC_12M       = CLKGEN_CLKOUT_CKSEL_HFRC_DIV8,    // HFRC / 8      = 12MHz
    AM_HAL_CLKGEN_CLKOUT_HFRC_6M        = CLKGEN_CLKOUT_CKSEL_HFRC_DIV16,   // HFRC / 16     = 6MHz
    AM_HAL_CLKGEN_CLKOUT_HFRC_3M        = CLKGEN_CLKOUT_CKSEL_HFRC_DIV32,   // HFRC / 32     = 3MHz
    AM_HAL_CLKGEN_CLKOUT_HFRC_750K      = CLKGEN_CLKOUT_CKSEL_HFRC_DIV128,  // HFRC / 128    = 750KHz
    AM_HAL_CLKGEN_CLKOUT_HFRC_375K      = CLKGEN_CLKOUT_CKSEL_HFRC_DIV256,  // HFRC / 256    = 375KHz
    AM_HAL_CLKGEN_CLKOUT_HFRC_187K      = CLKGEN_CLKOUT_CKSEL_HFRC_DIV512,  // HFRC / 512    = 187.5KHz
    AM_HAL_CLKGEN_CLKOUT_HFRC_93750     = CLKGEN_CLKOUT_CKSEL_HFRC_DIV1024, // HFRC / 1024   = 93.75KHz
    AM_HAL_CLKGEN_CLKOUT_HFRC_366       = CLKGEN_CLKOUT_CKSEL_HFRC_DIV256K, // HFRC / 262144 = 366.2Hz
    AM_HAL_CLKGEN_CLKOUT_HFRC_1P4       = CLKGEN_CLKOUT_CKSEL_HFRC_DIV64M,  // HFRC / 64M    = 1.4Hz

    AM_HAL_CLKGEN_CLKOUT_HFRC2_24M      = CLKGEN_CLKOUT_CKSEL_HFRC2_24MHz,  // HFRC2        = 24MHz
    AM_HAL_CLKGEN_CLKOUT_HFRC2_12M      = CLKGEN_CLKOUT_CKSEL_HFRC2_12MHz,  // HFRC2        = 12MHz
    AM_HAL_CLKGEN_CLKOUT_HFRC2_6M       = CLKGEN_CLKOUT_CKSEL_HFRC2_6MHz,   // HFRC2        = 6MHz

    // Uncalibrated LFRC
    AM_HAL_CLKGEN_CLKOUT_ULFRC_64       = CLKGEN_CLKOUT_CKSEL_ULFRC_DIV16,  // ULFRC / 16   = 64 Hz (uncal LFRC)
    AM_HAL_CLKGEN_CLKOUT_ULFRC_8        = CLKGEN_CLKOUT_CKSEL_ULFRC_DIV128, // ULFRC / 128  =  8 Hz (uncal LFRC)
    AM_HAL_CLKGEN_CLKOUT_ULFRC_1        = CLKGEN_CLKOUT_CKSEL_ULFRC_1Hz,    // ULFRC / 1024 =  1 Hz (uncal LFRC)
    AM_HAL_CLKGEN_CLKOUT_ULFRC_0_25     = CLKGEN_CLKOUT_CKSEL_ULFRC_DIV4K,  // ULFRC / 4096 = 0.25 Hz (uncal LFRC)
    AM_HAL_CLKGEN_CLKOUT_ULFRC_0_0009   = CLKGEN_CLKOUT_CKSEL_ULFRC_DIV1M,  // ULFRC / 1M   = 0.000976 Hz (uncal LFRC)

    // Not Autoenabled ("NE")
    AM_HAL_CLKGEN_CLKOUT_XTALNE_32768   = CLKGEN_CLKOUT_CKSEL_XTNE,         // XTALNE / 1   = 32768 Hz
    AM_HAL_CLKGEN_CLKOUT_XTALNE_2048    = CLKGEN_CLKOUT_CKSEL_XTNE_DIV16,   // XTALNE / 16  =  2048 Hz
    AM_HAL_CLKGEN_CLKOUT_LFRCNE         = CLKGEN_CLKOUT_CKSEL_LFRCNE,       // LFRCNE / 32  =    32 Hz
    AM_HAL_CLKGEN_CLKOUT_LFRCNE_32      = CLKGEN_CLKOUT_CKSEL_LFRCNE_DIV32, // LFRCNE / 32  =    32 Hz

    // Misc clocks
    AM_HAL_CLKGEN_CLKOUT_RTC_1HZ        = CLKGEN_CLKOUT_CKSEL_RTC_1Hz,      // RTC
    AM_HAL_CLKGEN_CLKOUT_CG_100         = CLKGEN_CLKOUT_CKSEL_CG_100Hz,     // ClkGen 100Hz
}
am_hal_clkgen_clkout_e;

#define AM_HAL_CLKGEN_CLKOUT_MAX        CLKGEN_CLKOUT_CKSEL_HFRC2_24MHz     // Highest valid CKSEL enum value

//
//! enum for HFRC2 FLL computation
//
typedef enum
{
    //
    //! compute the hf2adj parameters from input and output freqs.
    //
    AM_HAL_CLKGEN_HF2ADJ_COMP_COMP_FREQ = 0x01,
    //
    //! use passed in values directory
    //
    AM_HAL_CLKGEN_HF2ADJ_COMP_DIRECT_ARG = 0x02,

    //
    //! when set the HFRC2ADJ is started during the HFRC2 adjust resample
    //
    AM_HAL_CLKGEN_HFRC2ADJ_RECOMPUTE_START = 0x04,
    //
    //! when set the HFRC2ADJ is sampled during the HFRC2 adjust resample
    //
    AM_HAL_CLKGEN_HFRC2ADJ_RECOMPUTE_SAMPLE = 0x08,
    //
    //! both start hfrc2 and sample (combination of two bits)
    //
    AM_HAL_CLKGEN_HFRC2ADJ_START_AND_SAMPLE = AM_HAL_CLKGEN_HFRC2ADJ_RECOMPUTE_START | AM_HAL_CLKGEN_HFRC2ADJ_RECOMPUTE_SAMPLE,

    //
    //! force this enum to be sizeof 4 bytes
    //
    AM_HAL_CLKGEN_HF2ADJ_COMP_ALIGH = 0x70000000,

}
am_hal_clockgen_hf2adj_compute_e;

//
//! struct used to pass data for AM_HAL_CLKGEN_CONTROL_HF2ADJ_COMPUTE
//
typedef struct
{
    //
    //!
    //
    am_hal_clockgen_hf2adj_compute_e eHF2AdjType;
    //
    //! the xref oscillator frequency in hz
    //
    uint32_t ui32Source_freq_in_hz;
    //
    //! the target(output) frequency in hz
    //
    uint32_t ui32Target_freq_in_hz;

    //
    // !This is the un-shifted value of register HF2ADJ1.HF2ADJTRIMOFFSET
    // !This is an 11 bit signed value (max)
    //
    uint32_t ui32TrimValue;    // max is 0x7FFF

    //
    //! the max value for this is 7, if this is > 7 or 0
    //! the default value of 7 will be used
    //
    uint8_t ui8TrimSetting;

    //
    //! HFRC2-adj is default enabled,
    //! set this true to prevent HFRC2-adj being enabled
    //
    bool    bDoNotEnableHFRC2;

    //
    //! struct length align to 4 byte multiple
    //
    uint8_t reserved_1[2];
}
am_hal_clockgen_hf2adj_compute_t;

//
//! struct used to pass data for AM_HAL_CLKGEN_CONTROL_HF2ADJ_RECOMPUTE
//
typedef struct
{
    //
    //! specify HFRC2 adjust enable, sample and update, or both
    //!  -- that is
    //!  -- AM_HAL_CLKGEN_HFRC2ADJ_RECOMPUTE_START  (Section 1)
    //!  -- AM_HAL_CLKGEN_HFRC2ADJ_RECOMPUTE_SAMPLE  (Section 2)
    //!  -- AM_HAL_CLKGEN_HFRC2ADJ_START_AND_SAMPLE  (Section 1 and Section 2)
    //
    am_hal_clockgen_hf2adj_compute_e eHF2RecomputeControl;

    //
    //! the number of samples to average
    //
    uint32_t ui32NumSamples;

    //
    //! This is the delay between restarting HFRC2-adjust (Section1)
    //! and sampling it's output(Section2).
    //!
    //! It is recommended there be at least 1000usec delay between
    //! Section 1 and Section 2
    //
    uint32_t ui32StartupDelay_us;

    //
    //! the delay between samples
    //! it is recommended this value be at least 1
    //
    uint32_t ui32SampleDelay_us;    // max is 0x7FFF

    //
    //! if the user wants to disable the XTHS and this
    //! pointer is provided, this will be used for api calls
    //! to enable and disable the 32Mhz clock(XTHS)
    //! this cannot be on the stack.
    //! @Todo check if this is on the stack on init one-time?
    //! is the a good way to check for stack variable
    //!

    am_hal_mcuctrl_control_arg_t mcu_ctrl_xths_controlArg;

    //
    //! 32Mhz clock startup delay
    //! it is recommended this value be at least 200usec
    //
    uint32_t ui32Post32MhzRestartDelay_us;

    //
    //! Disable the 32Mhz XTALHS between HFRC2 tune samples sessions
    //
    bool    bDisableXTALHS;

    //
    //! struct align to 4 byte multiple
    //
    uint8_t  align[2];

}
am_hal_clockgen_hf2adj_recompute_t;


//
//! Status structure.
//
typedef struct
{
    //
    // ui32SysclkFreq
    //!  Returns the current system clock frequency, in hertz.
    //
    uint32_t    ui32SysclkFreq;

    //
    // eRTCOSC
    //
    //!  Returns the current RTC oscillator as one of:
    //!  AM_HAL_CLKGEN_STATUS_RTCOSC_LFRC
    //!  AM_HAL_CLKGEN_STATUS_RTCOSC_XTAL
    //
    uint32_t    eRTCOSC;

    //
    // bXtalFailure
    //!  true = XTAL has failed (is enabled but not oscillating).  Also if the
    //!         LFRC is selected as the oscillator in OCTRL.OSEL.
    //
    bool        bXtalFailure;

    //
    // enable status for all the peripheral clocks.
    //  1: enable
    //  0: disable
    //
    //uint32_t    ui32Clockenstat;
    //uint32_t    ui32Clocken2stat;
    //uint32_t    ui32Clocken3stat;
}
am_hal_clkgen_status_t;

extern const am_hal_clockgen_hf2adj_recompute_t tReComputeCtrlDefault;
extern const am_hal_clockgen_hf2adj_recompute_t tReComputeCtrlNo32MhzDefault;

// ****************************************************************************
//
//! @brief Apply various specific commands/controls on the CLKGEN module.
//!
//! This function is used to apply various controls on CLKGEN.
//!
//! @note IMPORTANT! This function MUST be called very early in execution of
//!       an application with the parameter AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX
//!       in order to set Apollo4l to its required operating frequency.
//!
//! @param eControl - One of the following:
//!     AM_HAL_CLKGEN_CONTROL_RTC_SEL_LFRC
//!     AM_HAL_CLKGEN_CONTROL_RTC_SEL_XTAL
//!     AM_HAL_CLKGEN_CONTROL_HFADJ_ENABLE
//!     AM_HAL_CLKGEN_CONTROL_HFADJ_DISABLE
//!     AM_HAL_CLKGEN_CONTROL_HF2ADJ_ENABLE
//!     AM_HAL_CLKGEN_CONTROL_HF2ADJ_DISABLE
//!     AM_HAL_CLKGEN_CONTROL_HFRC2_START
//!     AM_HAL_CLKGEN_CONTROL_HFRC2_STOP
//!     AM_HAL_CLKGEN_CONTROL_RESERVED1,
//!     AM_HAL_CLKGEN_CONTROL_RESERVED2,
//!     AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_OFF
//!     AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_HFRC48
//!     AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_HFRC96
//!     AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_DPHYPLL
//!     AM_HAL_CLKGEN_CONTROL_PLLCLK_ENABLE
//!     AM_HAL_CLKGEN_CONTROL_PLLCLK_DISABLE
//!     AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_OFF
//!     AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_HFRC12
//!     AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_HFRC6
//!     AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_HFXT
//!     AM_HAL_CLKGEN_CONTROL_HF2ADJ_COMPUTE
//! @param pArgs - Pointer to arguments for Control Switch Case
//!
//! @return status      - generic or interface specific status.
//!
//! @note After starting the XTAL, a 2 second warm-up delay is required.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_control(am_hal_clkgen_control_e eControl,
                                      void *pArgs);

// ****************************************************************************
//
//! @brief Get CLKGEN status.
//!
//! This function returns the current value of various CLKGEN statuses.
//!
//! @param psStatus - ptr to a status structure to receive the current statuses.
//!
//! @return status      - generic or interface specific status.
//!
//! @note After selection of the RTC Oscillator, a 2 second delay is required
//! before the new oscillator takes effect.  Therefore the CLKGEN.STATUS.OMODE
//! bit will not reflect the new status until after the 2s wait period.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_status_get(am_hal_clkgen_status_t *psStatus);

// ****************************************************************************
//
//! @brief Enable CLKOUT.
//!
//! This function is used to enable and select a CLKOUT frequency.
//!
//! @param bEnable: true to enable, false to disable.
//! @param eClkSelect - One of the following:
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_32768
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_16384
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_8192
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_4096
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_2048
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_1024
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_128
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_4
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_0_5
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_0_015
//!
//!     AM_HAL_CLKGEN_CLKOUT_LFRC
//!     AM_HAL_CLKGEN_CLKOUT_LFRC_512
//!     AM_HAL_CLKGEN_CLKOUT_LFRC_32
//!     AM_HAL_CLKGEN_CLKOUT_LFRC_2
//!     AM_HAL_CLKGEN_CLKOUT_LFRC_0_03
//!     AM_HAL_CLKGEN_CLKOUT_LFRC_0_0010
//!
//!     AM_HAL_CLKGEN_CLKOUT_HFRC_48M
//!     AM_HAL_CLKGEN_CLKOUT_HFRC_12M
//!     AM_HAL_CLKGEN_CLKOUT_HFRC_6M
//!     AM_HAL_CLKGEN_CLKOUT_HFRC_3M
//!     AM_HAL_CLKGEN_CLKOUT_HFRC_750K
//!     AM_HAL_CLKGEN_CLKOUT_HFRC_375K
//!     AM_HAL_CLKGEN_CLKOUT_HFRC_187K
//!     AM_HAL_CLKGEN_CLKOUT_HFRC_93750
//!     AM_HAL_CLKGEN_CLKOUT_HFRC_366
//!     AM_HAL_CLKGEN_CLKOUT_HFRC_1P4
//!     AM_HAL_CLKGEN_CLKOUT_HFRC2_24M
//!     AM_HAL_CLKGEN_CLKOUT_HFRC2_12M
//!     AM_HAL_CLKGEN_CLKOUT_HFRC2_6M
//!
//!     // Uncalibrated LFRC
//!     AM_HAL_CLKGEN_CLKOUT_ULFRC_64
//!     AM_HAL_CLKGEN_CLKOUT_ULFRC_8
//!     AM_HAL_CLKGEN_CLKOUT_ULFRC_1
//!     AM_HAL_CLKGEN_CLKOUT_ULFRC_0_25
//!     AM_HAL_CLKGEN_CLKOUT_ULFRC_0_0009
//!
//!     // Not Autoenabled ("NE")
//!     AM_HAL_CLKGEN_CLKOUT_XTALNE_32768
//!     AM_HAL_CLKGEN_CLKOUT_XTALNE_2048
//!     AM_HAL_CLKGEN_CLKOUT_LFRCNE
//!     AM_HAL_CLKGEN_CLKOUT_LFRCNE_32
//!
//!     // Misc clocks
//!     AM_HAL_CLKGEN_CLKOUT_RTC_1HZ
//!     AM_HAL_CLKGEN_CLKOUT_CG_100
//!     AM_HAL_CLKGEN_CLKOUT_FLASHCLK
//!
//! @return status      - generic or interface specific status.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_clkout_enable(bool bEnable,
                                            am_hal_clkgen_clkout_e eClkSelect);

//*****************************************************************************
//
//! @brief Setup hfrc2 adjust based on input values
//!
//! @param psHf2Cfg  am_hal_clockgen_hf2adj_compute_t containing configuration settings
//!
//! @return standard hal status
//
//*****************************************************************************
extern uint32_t am_hal_hfrc2_adj_control( const am_hal_clockgen_hf2adj_compute_t *psHf2Cfg);

//*****************************************************************************
//
//! @brief sample the HFRC2-adjust output and use enable as a fixed output.
//!
//! @details This is intended to reduce jitter from HFRC2 adjust.
//! This code will:\n
//!     1. Section 1\n
//!         - start the HFRC2-adjust\n
//!         - wait a programmable amount of time for HFRC2-adjust startup\n
//!             - this delay should be at least 1000 usec\n
//!     2. Section 2\n
//!         - sample a programmable number of adjust mux output values\n
//!             - Use a programmable intersample delay  (minimum 1 usec)\n
//!             - sign-extend and average the samples\n
//!         - save the averaged value to the tune register\n
//!         - disable HFRC2 adjust\n
//!
//! @note It is recommended this function be called every 10 seconds.
//!
//! @note Section 1 and 2 can be run independently depending on eSampleControl.
//!
//! @param psRecomputeCtrl  am_hal_clockgen_hf2adj_recompute_t see structure definition
//!                          for details and recommendations of each field.
//!
//! @return Generic success status.
//
//*****************************************************************************
extern uint32_t am_hal_clkgen_HFRC2_adj_recompute( const am_hal_clockgen_hf2adj_recompute_t *psRecomputeCtrl);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_CLKGEN_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

