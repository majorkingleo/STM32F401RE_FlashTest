/*
 * Generic driver class that can write unaligned to any
 * flash page. Will read necessary data from the page before.
 * Do call automatically erase, before writing.
 *
 * @author Copyright (c) 2024 Martin Oberzalek
 */

#ifndef APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_GENERICFLASHDRIVER_H_
#define APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_GENERICFLASHDRIVER_H_

#include "RawDriverInterface.h"
#include "MemoryInterface.h"

namespace stm32_internal_flash {

class GenericFlashDriver : public MemoryInterface
{
protected:
	RawDriverInterface & raw_driver;
	std::span<std::byte> * page_buffer; // buffer that has to be PAGE_SIZE size

public:
	/**
	 * page_buffer: span that has to be PAGE_SIZE large. If null alloca is used
	 *              to get the buffer on stack. Be careful, this can be a large value.
	 */
	GenericFlashDriver( RawDriverInterface & raw_driver_, std::span<std::byte> * page_buffer_ = nullptr )
	: raw_driver( raw_driver_ ),
	  page_buffer( page_buffer_ )
	{}

	std::size_t get_size() const override;
	std::size_t get_page_size() const override;

	/**
	 * writes data, aligned or unaligned. Erase is called automatically before
	 */
	std::size_t write( std::size_t address, const std::span<std::byte> & data ) override;
	std::size_t read( std::size_t address, std::span<std::byte> & data ) override;

	bool erase( std::size_t address, std::size_t size ) override;

protected:
	/**
	 * writes an unaligned amount of data, by reading the required page data before
	 * data.size() has to be <= PAGE_SIZE
	 * returns the amount of new data written, which should be data.size()
	 */
	std::size_t write_unaligned_first_page( std::size_t address, const std::span<std::byte> & data );

	std::size_t write_unaligned_last_page( std::size_t address, const std::span<std::byte> & data );
};

}

#endif /* APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_GENERICFLASHDRIVER_H_ */
