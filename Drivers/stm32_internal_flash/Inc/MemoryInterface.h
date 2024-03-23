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
};

} // namespace smt32_internal_flash

#endif /* APP_STM32_INTERNAL_FLASH_INC_MEMORYINTERFACE_H_ */
