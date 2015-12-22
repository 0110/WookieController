/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio
    LPC11xx EXT driver - Copyright (C) 2013 Marcin Jokel
                       - Copyright (C) 2013 mike brown

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    LPC11xx/ext_lld.h
 * @brief   LPC11xx EXT subsystem low level driver header.
 *
 * @addtogroup EXT
 * @{
 */

#ifndef _EXT_LLD_H_
#define _EXT_LLD_H_

#if HAL_USE_EXT || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @brief   Available number of EXT channels.
 */
#define EXT_MAX_CHANNELS    12


/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @brief   EXT0 driver enable switch.
 * @details If set to @p TRUE the support for EXT0 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(LPC11xx_EXT_USE_EXT0) || defined(__DOXYGEN__)
#define LPC11xx_EXT_USE_EXT0              FALSE
#endif

/**
 * @brief   EXT1 driver enable switch.
 * @details If set to @p TRUE the support for EXT1 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(LPC11xx_EXT_USE_EXT1) || defined(__DOXYGEN__)
#define LPC11xx_EXT_USE_EXT1              FALSE
#endif

/**
 * @brief   EXT2 driver enable switch.
 * @details If set to @p TRUE the support for EXT2 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(LPC11xx_EXT_USE_EXT2) || defined(__DOXYGEN__)
#define LPC11xx_EXT_USE_EXT2              FALSE
#endif

/**
 * @brief   EXT3 driver enable switch.
 * @details If set to @p TRUE the support for EXT3 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(LPC11xx_EXT_USE_EXT3) || defined(__DOXYGEN__)
#define LPC11xx_EXT_USE_EXT3              FALSE
#endif
/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   EXT channel identifier.
 */
typedef uint32_t expchannel_t;

/**
 * @brief   Type of an EXT generic notification callback.
 *
 * @param[in] extp      pointer to the @p EXPDriver object triggering the
 *                      callback
 */
typedef void (*extcallback_t)(EXTDriver *extp, expchannel_t channel);

/**
 * @brief   Channel configuration structure.
 */
typedef struct {
  /**
   * @brief Channel mode.
   */
  uint8_t               mode;

  /**
   * @brief Channel callback.
   */
  extcallback_t         cb;
} EXTChannelConfig;

/**
 * @brief   Driver configuration structure.
 * @note    It could be empty on some architectures.
 */
typedef struct {
  /**
   * @brief Channel configurations.
   */
  EXTChannelConfig      channels[EXT_MAX_CHANNELS];
  /* End of the mandatory fields.*/
} EXTConfig;

/**
 * @brief   Structure representing an EXT driver.
 */
struct EXTDriver {
  /**
   * @brief Driver state.
   */
  extstate_t                state;
  /**
   * @brief Current configuration data.
   */
  const EXTConfig           *config;
  /* End of the mandatory fields.*/
  LPC_GPIO_TypeDef          *gpio;
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if LPC11xx_EXT_USE_EXT0 || !defined(__DOXYGEN__)
extern EXTDriver EXTD0;
#endif

#if LPC11xx_EXT_USE_EXT1 || !defined(__DOXYGEN__)
extern EXTDriver EXTD1;
#endif

#if LPC11xx_EXT_USE_EXT2 || !defined(__DOXYGEN__)
extern EXTDriver EXTD2;
#endif

#if LPC11xx_EXT_USE_EXT3 || !defined(__DOXYGEN__)
extern EXTDriver EXTD3;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void ext_lld_init(void);
  void ext_lld_start(EXTDriver *extp);
  void ext_lld_stop(EXTDriver *extp);
  void ext_lld_channel_enable(EXTDriver *extp, expchannel_t channel);
  void ext_lld_channel_disable(EXTDriver *extp, expchannel_t channel);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_EXT */

#endif /* _EXT_LLD_H_ */

/** @} */
