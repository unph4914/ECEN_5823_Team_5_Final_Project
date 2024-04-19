#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_adc.h"

#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

bool ADC_flag = false;

#define adcFreq   13000000

volatile uint32_t sample;
volatile uint32_t millivolts;

/**************************************************************************//**
 * @brief  Initialize ADC function
 *****************************************************************************/
static void delayApprox(int delay)
{
  volatile int i;

  for (i = 0; i < delay; ) {
      i=i+1;
  }

} // delayApprox()

void initADC (void)
{
  // Enable ADC0 clock
  CMU_ClockEnable(cmuClock_ADC0, true);
  CMU_ClockEnable(cmuClock_GPIO, true);
  LOG_INFO("ADC0 and GPIO clocks enabled.\r\n");

  GPIO_PinModeSet(gpioPortD, 10, gpioModePushPull, 0);
  LOG_INFO("GPIO PD10 configured as Push-Pull for the buzzer.\r\n");

//  LOG_INFO("ADC0 clock enabled.\r\n");

  // Declare init structs
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;

  // Modify init structs and initialize
  init.prescale = ADC_PrescaleCalc(adcFreq, 0); // Init to max ADC clock for Series 0
  init.timebase = ADC_TimebaseCalc(0);

  initSingle.diff       = false;        // single ended
  initSingle.reference  = adcRef2V5;    // internal 2.5V reference
  initSingle.resolution = adcRes12Bit;  // 12-bit resolution

  // Select ADC input. See README for corresponding EXP header pin.
  initSingle.posSel = adcPosSelAPORT2XCH9;

  ADC_Init(ADC0, &init);
  ADC_InitSingle(ADC0, &initSingle);
  LOG_INFO("ADC initialized for single-ended operation with 12-bit resolution.\r\n");

  // Enable ADC Single Conversion Complete interrupt
  ADC_IntEnable(ADC0, ADC_IEN_SINGLE);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(ADC0_IRQn);
  NVIC_EnableIRQ(ADC0_IRQn);
}

/**************************************************************************//**
 * @brief  ADC Handler
 *****************************************************************************/
void ADC0_IRQHandler(void)
{
  LOG_INFO("ADC IRQ Handler triggered.\r\n");
  // Clear the interrupt flag
  ADC_IntClear(ADC0, ADC_IF_SINGLE);

  delayApprox(300000);

  // Get ADC result
  sample = ADC_DataSingleGet(ADC0);

  // Calculate input voltage in mV
  millivolts = (sample * 2500) / 4096;

  // Print
  LOG_INFO("ADC data received: %u mV\r\n", millivolts);

  if (millivolts < 1000)
  {
      GPIO_PinOutSet(gpioPortD, 10);
      LOG_INFO("Buzzer ON due to low light intensity.\r\n");
  }
  else
  {
      GPIO_PinOutClear(gpioPortD, 10);
      LOG_INFO("Buzzer OFF due to sufficient light intensity.\r\n");
  }

  // Start next ADC conversion
  ADC_Start(ADC0, adcStartSingle);
}
