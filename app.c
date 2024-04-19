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

#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

bool ADC_flag = false;

void app_init(void)
{
    CMU_ClockEnable(cmuClock_ADC0, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    LOG_INFO("ADC0 and GPIO clocks enabled.\r\n");

    GPIO_PinModeSet(gpioPortD, 10, gpioModePushPull, 0);
    LOG_INFO("GPIO PD10 configured as Push-Pull for the buzzer.\r\n");

    ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
    ADC_InitSingle_TypeDef sInit = ADC_INITSINGLE_DEFAULT;

    init.prescale = ADC_PrescaleCalc(16000000, 0);

    sInit.diff = false;
    sInit.reference = adcRef2V5;
    sInit.resolution = adcRes12Bit;
    sInit.acqTime = adcAcqTime4;

    sInit.posSel = adcPosSelAPORT2XCH9;

    ADC_Init(ADC0, &init);
    ADC_InitSingle(ADC0, &sInit);
    LOG_INFO("ADC initialized for single-ended operation with 12-bit resolution.\r\n");
}

void app_process_action(void)
{
    NVIC_EnableIRQ(ADC0_IRQn);
    ADC_IntEnable(ADC0, ADC_IF_SINGLE);
    ADC_Start(ADC0, adcStartSingle);
    uint16_t sensor_data = 0;

    if (ADC_flag)
    {
        uint32_t ADC_data = ADC_DataSingleGet(ADC0);
        sensor_data = (ADC_data * 2500) / 4096;
        LOG_INFO("ADC data received: %u mV\r\n", sensor_data);

        if (sensor_data < 1000)
        {
            GPIO_PinOutSet(gpioPortD, 10);
            LOG_INFO("Buzzer ON due to low light intensity.\r\n");
        }
        else
        {
            GPIO_PinOutClear(gpioPortD, 10);
            LOG_INFO("Buzzer OFF due to sufficient light intensity.\r\n");
        }

        ADC_flag = false;
    }
}

void ADC0_IRQHandler(void)
{
    ADC_flag = true;
    NVIC_DisableIRQ(ADC0_IRQn);
    ADC_IntDisable(ADC0, ADC_IF_SINGLE);
    LOG_INFO("ADC IRQ Handler triggered.\r\n");
}
