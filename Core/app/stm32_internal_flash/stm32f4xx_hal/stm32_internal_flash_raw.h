/*
 * stm32_internal_flash_raw.h
 *
 *  Created on: Mar 3, 2024
 *      Author: Martin
 */

#ifndef APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_STM32_INTERNAL_FLASH_RAW_H_
#define APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_STM32_INTERNAL_FLASH_RAW_H_

#include "stm32_internal_flash.h"
#include "RawDriverInterface.h"

namespace smt32_internal_flash {

class STM32InternalFlashHalRaw : public RawDriverInterface
{
	Configuration & conf;
	std::optional<Error> error;

public:
	STM32InternalFlashHalRaw( Configuration & conf );

	bool operator!() const {
		return error.has_value();
	}

	/**
	 * erases on or more pages
	 * size has to be page size
	 */
	bool erase_page_by_page_startaddress( std::size_t page_start_address, std::size_t size );

	const std::optional<Error> & get_error() const {
		return error;
	}

	void clear_error() {
		error = {};
	}

	std::size_t write_page( std::size_t address, const std::span<std::byte> & buffer ) override;
	std::size_t read_page( std::size_t address, std::span<std::byte> & buffer )  override;

	std::size_t get_page_size() override;

	bool erase_page( std::size_t address, std::size_t size ) override;

private:
	std::optional<Configuration::Sector> get_sector_from_address( std::size_t address ) const;
	void clear_flags();
};

} // namespace smt32_internal_flash

#endif /* APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_STM32_INTERNAL_FLASH_RAW_H_ */
