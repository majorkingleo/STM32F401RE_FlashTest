/*
 * GenericFlashDriver.cpp
 *
 *  Created on: Mar 6, 2024
 *      Author: Martin
 */
#include "GenericFlashDriver.h"

namespace smt32_internal_flash {

std::size_t GenericFlashDriver::get_size() const
{
	return raw_driver.get_size();
}

std::size_t GenericFlashDriver::write( std::size_t address, const std::span<std::byte> & data )
{
	return 0;
}

std::size_t GenericFlashDriver::read( std::size_t address, std::span<std::byte> & data )
{
	return 0;
}

void GenericFlashDriver::erase( std::size_t address, std::size_t size )
{

}

} // namespace smt32_internal_flash
