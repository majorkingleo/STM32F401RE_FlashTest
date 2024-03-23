/*
 * JBODGenericFlashDriver.cpp
 *
 *  Created on: Mar 21, 2024
 *      Author: martin.oberzalek
 */
#include "JBODGenericFlashDriver.h"

namespace stm32_internal_flash {

std::size_t JBODGenericFlashDriver::get_size() const
{
	std::size_t size = 0;

	for( MemoryInterface* driver : drivers ) {
		size += driver->get_size();
	}

	return size;
}

std::size_t JBODGenericFlashDriver::get_page_size() const
{
	std::size_t max_page_size = 0;

	for( MemoryInterface* driver : drivers ) {
		max_page_size = std::max( max_page_size, driver->get_page_size() );
	}

	return max_page_size;
}

bool JBODGenericFlashDriver::erase( std::size_t address, std::size_t size )
{
	DriverInfo info = get_driver_idx_by_address( address );

	if( !info ) {
		return false;
	}

	address -= info.address_offset;

	for( unsigned idx = info.driver_idx; idx < drivers.size(); idx++ ) {
		MemoryInterface *driver = drivers[idx];

		std::size_t local_size = std::min( driver->get_size(), size );

		if( !driver->erase( address, local_size ) ) {
			return false;
		}

		size -= local_size;

		if( driver->get_size() > size ) {
			return true;
		}
	}

	return false;
}

MemoryInterface* JBODGenericFlashDriver::get_driver_by_address( std::size_t address ) const
{
	for( MemoryInterface* driver : drivers ) {
		if( driver->get_size() > address ) {
			return driver;
		}
		address -= driver->get_size();
	}

	return nullptr;
}

JBODGenericFlashDriver::DriverInfo JBODGenericFlashDriver::get_driver_idx_by_address( std::size_t address ) const
{
	DriverInfo info;
	std::size_t current_address = address;
	std::size_t address_offset = 0;

	for( std::size_t i = 0; i < drivers.size(); i++ ) {

		const std::size_t junk_size = drivers[i]->get_size();

		if( junk_size > address ) {
			info.driver = drivers[i];
			info.driver_idx = i;
			info.address_offset = address_offset;
			return info;
		}

		current_address -= junk_size;
		address_offset += junk_size;
	}

	return info;
}

std::size_t JBODGenericFlashDriver::write( std::size_t address, const std::span<std::byte> & data )
{
	DriverInfo info = get_driver_idx_by_address( address );

	if( !info ) {
		return false;
	}

	address -= info.address_offset;
	std::span<std::byte> local_data = data;
	std::size_t len = 0;

	for( unsigned idx = info.driver_idx; idx < drivers.size(); idx++ ) {
		MemoryInterface *driver = drivers[idx];

		std::size_t local_size = std::min( driver->get_size(), local_data.size() );
		std::span<std::byte> sub_data = local_data.subspan(0, local_size);

		std::size_t len_written = driver->write( address, sub_data );
		len += len_written;

		if( len_written != sub_data.size() ) {
			return len;
		}

		if( len >= data.size() ) {
			break;
		}

		local_data = local_data.subspan(local_data.size());
	}

	return len;
}

} // namespace stm32_internal_flash


