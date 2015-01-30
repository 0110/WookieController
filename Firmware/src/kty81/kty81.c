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

#define ADC_GRP1_VALUE_OFFSET   1

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

static volatile int32_t *gTemperature = NULL;

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/

/*
 * ADC conversion group.
 * Mode:        Linear buffer, 4 samples of 2 channels, SW triggered.
 * Channels:    IN11   (48 cycles sample time)
 *              Sensor (192 cycles sample time)
 */
static const ADCConversionGroup adcgrpcfg = {
  FALSE,
  ADC_GRP1_NUM_CHANNELS,
  adccb,
  NULL,
  /* HW dependent part.*/
  0,
  ADC_CR2_SWSTART,
  ADC_SMPR1_SMP_AN11(ADC_SAMPLE_56) | ADC_SMPR1_SMP_SENSOR(ADC_SAMPLE_144),
  0,
  ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),
  0,
  ADC_SQR3_SQ2_N(ADC_CHANNEL_IN11) | ADC_SQR3_SQ1_N(ADC_CHANNEL_SENSOR)
};

/*
 * ADC end conversion callback.
 * The PWM channels are reprogrammed using the latest ADC samples.
 * The latest samples are transmitted into a single SPI transaction.
 */
void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n) {

  (void) buffer; (void) n;
  int i;
  uint32_t tempValue = 0;
  uint32_t tempResistorValue = 0;

  /* Note, only in the ADC_COMPLETE state because the ADC driver fires an
     intermediate callback when the buffer is half full.*/
  if (adcp->state == ADC_COMPLETE) {

    /*FIXME Remove the toggle LED */
    palTogglePad(GPIOD, GPIOD_LED5);    /* Red On*/


    /* Print out the old sampeled values */
    for (i=0; i < ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH; i++)
    {
          tempValue += samples[i];
    }

    if (gTemperature != NULL)
      (*gTemperature) = (int32_t) (tempValue / (ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH));
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

kty81_ret_t kty81_read(volatile int32_t *temperature)
{
  gTemperature = temperature;

  if (gTemperature == NULL)
    return RET_ERROR;

  /* Starts an asynchronous ADC conversion operation, the conversion
         will be executed in parallel to the current PWM cycle and will
         terminate before the next PWM cycle.*/
  chSysLockFromIsr();
  adcStartConversionI(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);
  chSysUnlockFromIsr();
  return RET_OK;
}
