/*
 * stm32_internal_flash.h
 *
 *  Created on: Mar 3, 2024
 *      Author: Martin
 */

#ifndef APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_STM32_INTERNAL_FLASH_H_
#define APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_STM32_INTERNAL_FLASH_H_

#include "../Inc/MemoryInterface.h"

namespace smt32_internal_flash {

class STM32InternalFlashHal : public MemoryInterface
{
public:
	STM32InternalFlashHal()
};

} // namespace namespace smt32_internal_flash {

#endif /* APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_STM32_INTERNAL_FLASH_H_ */
