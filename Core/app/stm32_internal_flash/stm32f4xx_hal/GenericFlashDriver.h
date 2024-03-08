/*
 * GenericFlashDriver.h
 *
 *  Created on: Mar 6, 2024
 *      Author: Martin
 */

#ifndef APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_GENERICFLASHDRIVER_H_
#define APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_GENERICFLASHDRIVER_H_

#include "RawDriverInterface.h"
#include "../Inc/MemoryInterface.h"

namespace smt32_internal_flash {

class GenericFlashDriver : public MemoryInterface
{
protected:
	RawDriverInterface & raw_driver;

public:
	GenericFlashDriver( RawDriverInterface & raw_driver_ )
	: raw_driver( raw_driver_ )
	{}

	std::size_t get_size() const override;
	std::size_t get_page_size() const override;

	std::size_t write( std::size_t address, const std::span<std::byte> & data ) override;
	std::size_t read( std::size_t address, std::span<std::byte> & data ) override;

	bool erase( std::size_t address, std::size_t size ) override;

protected:
	/**
	 * writes an unaligned amount of data, by reading the required page data before
	 * data.size() has to be <= PAGE_SIZE
	 */
	std::size_t write_unaligned_first_page( std::size_t address, const std::span<std::byte> & data );

	std::size_t write_unaligned_last_page( std::size_t address, const std::span<std::byte> & data );
};

}

#endif /* APP_STM32_INTERNAL_FLASH_STM32F4XX_HAL_GENERICFLASHDRIVER_H_ */
