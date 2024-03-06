/*
 * RawDriverInterface.h
 *
 *  Created on: Mar 6, 2024
 *      Author: Martin
 */

#ifndef APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_RAWDRIVERINTERFACE_H_
#define APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_RAWDRIVERINTERFACE_H_

#include <cstddef>
#include <span>

namespace smt32_internal_flash {

class RawDriverInterface
{
public:
	virtual ~RawDriverInterface() {}

	virtual std::size_t get_page_size() = 0;

	virtual bool erase_page( std::size_t address, std::size_t size ) = 0;

	virtual std::size_t write_page( std::size_t address, const std::span<std::byte> & buffer ) = 0;

	virtual std::size_t read_page( std::size_t address, std::span<std::byte> & buffer ) = 0;
};

} // namespace smt32_internal_flash

#endif /* APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_RAWDRIVERINTERFACE_H_ */
