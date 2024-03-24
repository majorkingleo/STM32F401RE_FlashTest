/*
 * Raw driver. Expects that all operations
 * are page aligned and only full pages can be written.
 *
 * This driver is using the HAL library.
 *
 * FOR STM32F4xxx processors:
 * No HAL_Init() is required, no SystemClock_Config() or SystemInit()
 * The driver and all function are working out of the box.
 * SystemClock_Config() would be required if calls should be able to run into a timeout
 *
 * @author Copyright (c) 2024 Martin Oberzalek
 */

#ifndef APP_STM32_INTERNAL_FLASH_STM32FXXX_HAL_STM32_INTERNAL_FLASH_RAW_H_
#define APP_STM32_INTERNAL_FLASH_STM32FXXX_HAL_STM32_INTERNAL_FLASH_RAW_H_

#include "stm32_internal_flash.h"
#include "RawDriverInterface.h"

namespace stm32_internal_flash {

class STM32InternalFlashHalRaw : public RawDriverInterface
{
	Configuration & conf;
	std::optional<Error> error;

public:
	STM32InternalFlashHalRaw( Configuration & conf );

	bool operator!() const {
		return error.has_value();
	}

	std::size_t get_size() override {
		return conf.size;
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

	/*
	 * Writes a page to flash, the memory has to be erased before.
	 * The function is not doing this automatically.
	 * After a write operation HAL driver is flushing the caches, of course there can be program code located.
	 *
	 * Buffer: can be smaller then page size.
	 */
	std::size_t write_page( std::size_t address, const std::span<std::byte> & buffer ) override;

	/**
	 * reads from flash, it's just a memcpy, since reading goes via internal addresses, so
	 * start address has not to be page aligned and
	 * buffer size can be smaller or larger than a page.
	 */
	std::size_t read_page( std::size_t address, std::span<std::byte> & buffer )  override;

	std::size_t get_page_size() override;

	/**
	 * Erases at least one page. size has to be a multiple of PAGE_SIZE
	 */
	bool erase_page( std::size_t address, std::size_t size ) override;

private:
	std::optional<Configuration::Sector> get_sector_from_address( std::size_t address ) const;
	void clear_flags();
};

} // namespace smt32_internal_flash

#endif /* APP_STM32_INTERNAL_FLASH_STM32FXXX_HAL_STM32_INTERNAL_FLASH_RAW_H_ */
