/***************************************************************************//**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "em_chip.h"
#include "em_cmu.h"
#include "em_adc.h"
#include "em_gpio.h"
#include "adc.h"

#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

static void delayApprox(int delay)
{
  volatile int i;

  for (i = 0; i < delay; ) {
      i=i+1;
  }

} // delayApprox()

void app_init(void)
{
    CHIP_Init();

    initADC();

    // Start first conversion
    ADC_Start(ADC0, adcStartSingle);
}

void app_process_action(void)
{

}
