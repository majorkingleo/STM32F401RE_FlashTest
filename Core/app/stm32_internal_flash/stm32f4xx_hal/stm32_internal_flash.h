/*
 * stm32_internal_flash.h
 *
 *  Created on: Mar 3, 2024
 *      Author: Martin
 */

#ifndef APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_STM32_INTERNAL_FLASH_H_
#define APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_STM32_INTERNAL_FLASH_H_

#include <optional>
#include <span>
#include <stdint.h>
#include <stm32f4xx_hal.h>

namespace smt32_internal_flash {

struct Configuration
{
	struct Sector
	{
		uint32_t sector{};
		uint32_t size{};
		std::size_t start_address = 0;
	};

	std::span<Sector> used_sectors{};
	uint32_t voltage_range = VOLTAGE_RANGE_3;
	uint32_t banks = FLASH_BANK_1;
	std::byte* data_ptr = nullptr;
};

class Error
{
public:
	enum ErrorCode
	{
		InvalidSectorAddress,
		ErrorUnlockingFlash,
		ErrorErasingFlash,
		HAL_Error
	};

	ErrorCode driver_error;
	uint32_t hal_error{};
public:

	Error( ErrorCode driver_error_, uint32_t hal_error_ = 0 )
	: driver_error( driver_error_ ),
	  hal_error(hal_error_)
	{}
};


class STM32InternalFlashHal
{
	Configuration & conf;
	std::optional<Error> error;

public:
	STM32InternalFlashHal( Configuration & conf );

	/**
	 * erases on or more pages
	 * size has to be page size
	 */
	bool erase_page( std::size_t address, std::size_t size );

	const std::optional<Error> & get_error() const {
		return error;
	}

	void clear_error() {
		error = {};
	}

	std::size_t write_page( std::size_t address, const std::span<std::byte> & buffer );

private:
	std::optional<Configuration::Sector> get_sector_from_address( std::size_t address ) const;
	void clear_flags();
};

} // namespace namespace smt32_internal_flash {

#endif /* APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_STM32_INTERNAL_FLASH_H_ */
