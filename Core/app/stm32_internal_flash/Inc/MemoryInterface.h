/*
 * Exposed MemoryInterface of this driver
 *
 *  Created on: Mar 3, 2024
 *      Author: Martin
 */

#ifndef APP_STM32_INTERNAL_FLASH_INC_MEMORYINTERFACE_H_
#define APP_STM32_INTERNAL_FLASH_INC_MEMORYINTERFACE_H_

namespace smt32_internal_flash {

class MemoryInterface
{
public:
	virtual ~MemoryInterface() {}

	virtual std::size_t size() const = 0;

	virtual std::size_t write( std::size_t address, const std::byte *data, std::size_t size ) = 0;
	virtual std::size_t read( std::size_t address, std::byte *data, std::size_t size ) = 0;

	virtual void erase( std::size_t address, std::size_t size ) = 0;
};

} // namespace smt32_internal_flash

#endif /* APP_STM32_INTERNAL_FLASH_INC_MEMORYINTERFACE_H_ */
