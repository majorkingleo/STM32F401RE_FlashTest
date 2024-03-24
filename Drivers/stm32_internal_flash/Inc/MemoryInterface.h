/*
 * Exposed MemoryInterface of this driver
 * @author Copyright (c) 2024 Martin Oberzalek
 */

#ifndef APP_STM32_INTERNAL_FLASH_INC_MEMORYINTERFACE_H_
#define APP_STM32_INTERNAL_FLASH_INC_MEMORYINTERFACE_H_

#include <cstddef>
#include <span>

namespace stm32_internal_flash {

class MemoryInterface
{
public:
	virtual ~MemoryInterface() {}

	virtual std::size_t get_size() const = 0;

	virtual std::size_t get_page_size() const = 0;

	virtual std::size_t write( std::size_t address, const std::span<std::byte> & data ) = 0;
	virtual std::size_t read( std::size_t address, std::span<std::byte> & data ) = 0;

	virtual bool erase( std::size_t address, std::size_t size ) = 0;

	/**
	 * Since the flash drive can erase only whole pages,
	 * It's not possible writing only half of a page, without destroying all.
	 * To do this the driver will read the whole page before, manipulates the data
	 * in RAM, erases the page and writes back all.
	 *
	 * This requires at least PAGE_SIZE bytes of RAM.
	 *
	 * If you don't have so much RAM, or just don't need it you can disable
	 * this feature.
	 */
	virtual void restore_data_on_unaligned_writes( bool state ) {
	}

	/**
	 * return true, if this feature is available
	 */
	virtual bool can_restore_data_on_unaligned_writes() const {
		return false;
	}

	virtual bool get_restore_data_on_unaligned_writes() const {
		return false;
	}
};

} // namespace smt32_internal_flash

#endif /* APP_STM32_INTERNAL_FLASH_INC_MEMORYINTERFACE_H_ */
