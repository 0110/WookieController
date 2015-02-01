/*
 * @file kty81.c
 *
 *  @date Jan 26, 2015
 *  @author ollo
 */

#include "kty81.h"

#include "ch.h"
#include "hal.h"

/******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

/* Total number of channels to be sampled by a single ADC operation.*/
#define ADC_GRP1_NUM_CHANNELS   1

/* Depth of the conversion buffer, channels are sampled four times each.*/
#define ADC_GRP1_BUF_DEPTH      4

/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/

static void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n);

/******************************************************************************
 * LOCAL VARIABLES
 ******************************************************************************/

/*
 * ADC samples buffer.
 */
static adcsample_t samples[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];

static uint32_t gADCval = 0;

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/

/** @var adcgrpcfg
 * @brief Configuration for the ADC module
 * ADC conversion group.
 * Mode:        Linear buffer, 4 samples of 1 channels, SW triggered.
 * Channels:    IN11   (192 cycles sample time)
 */
static const ADCConversionGroup adcgrpcfg = {
  FALSE,
  ADC_GRP1_NUM_CHANNELS,
  adccb,
  NULL,
  /* HW dependent part.*/
  0,
  ADC_CR2_SWSTART,
  ADC_SMPR1_SMP_AN11(ADC_SAMPLE_56),
  0,
  ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),
  0,
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN11)
};

/*
 * ADC end conversion callback.
 * The PWM channels are reprogrammed using the latest ADC samples.
 * The latest samples are transmitted into a single SPI transaction.
 */
void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n) {

  (void) buffer; (void) n;
  int i;
  uint32_t adcValue = 0;

  /* Note, only in the ADC_COMPLETE state because the ADC driver fires an
     intermediate callback when the buffer is half full.*/
  if (adcp->state == ADC_COMPLETE) {

    /* Generate average */
    for (i=0; i < ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH; i++)
    {
          adcValue += samples[i];
    }
    gADCval = (adcValue / (ADC_GRP1_BUF_DEPTH));

  }
}

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

kty81_ret_t kty81_init(void)
{
#ifdef UPRINT
  UPRINT("Initialize ADC ...");
#endif
  /*
   * The pin PC1 on the port GPIOC is programmed as analog input.
   */
  adcStart(&ADCD1, NULL);
  adcSTM32EnableTSVREFE();
  palSetPadMode(GPIOC, GPIOC_PIN1, PAL_MODE_INPUT_ANALOG);

#ifdef UPRINT
  UPRINT( " Done\r\n");
#endif

   return RET_OK;
}

kty81_ret_t kty81_read(int32_t *temperature)
{
  uint32_t tempResistorValue = 0;
  int32_t tempValue = 0;
  float volatage=0.0f;

  if (temperature == NULL)
    return RET_ERROR;

  /* Starts an asynchronous ADC conversion operation, the conversion
         will be executed in parallel to the current PWM cycle and will
         terminate before the next PWM cycle.*/
  chSysLockFromIsr();
  adcStartConversionI(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);
  chSysUnlockFromIsr();
  chThdSleep(MS2ST(10));

  /* Use the volatage value to convert the ADC value to a floating point number */
  volatage = (double) (gADCval * 1.0);
  volatage = (KTY81_ADC2V_FACTOR * (volatage - KTY81_ADC2V_OFFSET) + KTY81_ADC2V_OFFSET);

  /* Calculate the actual resistor value */
  tempResistorValue = (uint32_t) ((KTY81_PULLUP_RESISTOR * volatage) / (KTY81_VCC - volatage));

  /** Use the polynom to get the temperature */
  tempValue = KTY81_TEMP_FACTOR_X3 * (tempResistorValue * tempResistorValue * tempResistorValue)
                  + KTY81_TEMP_FACTOR_X2 * (tempResistorValue * tempResistorValue)
                  + KTY81_TEMP_FACTOR_X1 * tempResistorValue
                  + KTY81_TEMP_OFFSET_X0;

  (*temperature) = (int32_t) tempValue;
  return RET_OK;
}
