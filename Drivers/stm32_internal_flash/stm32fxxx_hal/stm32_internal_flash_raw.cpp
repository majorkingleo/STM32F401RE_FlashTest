/*
 * Raw driver. Expects that all operations
 * are page aligned and only full pages can be written.
 *
 * This driver is using the HAL library.
 *
 * @author Copyright (c) 2024 Martin Oberzalek
 */
#include "stm32_internal_flash_raw.h"
#include <string.h>

namespace stm32_internal_flash {

namespace {
	class AutoLockFlash
	{
	public:
		~AutoLockFlash()
		{
			HAL_FLASH_Lock();
		}
	};
}

STM32InternalFlashHalRaw::STM32InternalFlashHalRaw( Configuration & conf_ )
: conf( conf_ )
{
	conf.init();

	if( !conf ) {
		error = Error( Error::ConfigurationError );
	}
}

bool STM32InternalFlashHalRaw::erase_page_by_page_startaddress( std::size_t address, std::size_t size )
{
	auto sector = get_sector_from_address( address );

	if( !sector ) {
		error = Error::InvalidSectorAddress;
		return false;
	}

	FLASH_EraseInitTypeDef EraseInitStruct {};
	uint32_t PAGEError = 0;

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.Banks     = conf.banks;
	EraseInitStruct.Sector	  = sector->sector;
	EraseInitStruct.NbSectors = size / sector->size;

	if( EraseInitStruct.NbSectors == 0 ) {
		 EraseInitStruct.NbSectors = 1;
	}

	EraseInitStruct.VoltageRange = conf.voltage_range;


	if( HAL_FLASH_Unlock() != HAL_OK) {
		error = Error(Error::ErrorUnlockingFlash);
		return false;
	}

	AutoLockFlash auto_lock_flash;

	clear_flags();

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) {
		error = Error(Error::ErrorErasingFlash);
		return false;
	}

	return true;
}

void STM32InternalFlashHalRaw::clear_flags()
{
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP |
			               FLASH_FLAG_OPERR |
						   FLASH_FLAG_WRPERR |
						   FLASH_FLAG_PGAERR |
						   FLASH_FLAG_PGSERR |
						   FLASH_FLAG_PGPERR |
#ifdef FLASH_SR_RDERR
						   FLASH_SR_RDERR |
#endif
						   FLASH_SR_BSY );
}

std::optional<Configuration::Sector> STM32InternalFlashHalRaw::get_sector_from_address( std::size_t address ) const
{
	for( const Configuration::Sector & sec : conf.used_sectors ) {
		if( address == sec.start_address ) {
			return sec;
		}
	}

	return {};
}

std::size_t STM32InternalFlashHalRaw::write_page( std::size_t address, const std::span<std::byte> & buffer )
{
	if( HAL_FLASH_Unlock() != HAL_OK) {
		error = Error(Error::ErrorUnlockingFlash);
		return 0;
	}

	AutoLockFlash auto_lock_flash;

	clear_flags();

	auto do_write = [this]( uint32_t TypeProgram, auto data_type, std::size_t address, const std::span<std::byte> & buffer ) {
		using data_t = decltype(data_type);
		constexpr uint32_t data_step_size = sizeof(data_t);
		std::size_t size_written = 0;
		const uint32_t start_offset = reinterpret_cast<uint32_t>(conf.data_ptr);

		for( uint32_t offset = 0; offset <= buffer.size() - data_step_size; offset += data_step_size ) {

			std::size_t target_address = start_offset + address + offset;
			data_t *source = reinterpret_cast<data_t*>(buffer.data() + offset);
			uint64_t aligned_source_data = *source;

			HAL_StatusTypeDef ret = HAL_FLASH_Program(TypeProgram,
					target_address,
					aligned_source_data );

			if( ret != HAL_OK ) {
				error = Error(Error::HAL_Error,HAL_FLASH_GetError());
				return size_written;
			}

			size_written += data_step_size;
		}

		return size_written;
	};

	std::size_t size_written = 0;

	// Double word programming requires an external power supply of 9V
/*
	if( buffer.size() % sizeof(uint64_t) == 0 ) {
		size_written = do_write( FLASH_TYPEPROGRAM_DOUBLEWORD, (uint64_t)1, address, buffer );
	}
*/
	if( buffer.size() % sizeof(uint32_t) == 0 ) {
		size_written = do_write( FLASH_TYPEPROGRAM_WORD,       (uint32_t)1, address, buffer );
	} else if( buffer.size() % sizeof(uint16_t) == 0 ) {
		size_written = do_write( FLASH_TYPEPROGRAM_HALFWORD,   (uint16_t)1, address, buffer );
	} else {
		size_written = do_write( FLASH_TYPEPROGRAM_BYTE,       (uint16_t)1, address, buffer );
	}

	return size_written;
}

std::size_t STM32InternalFlashHalRaw::read_page( std::size_t address, std::span<std::byte> & buffer )
{
	std::size_t data_size = std::min( buffer.size(), conf.size );

	std::byte* source_address = conf.data_ptr + address;

	memcpy( buffer.data(), source_address, data_size );

	return data_size;
}

std::size_t STM32InternalFlashHalRaw::get_page_size()
{
	return conf.used_sectors.begin()->size;
}

bool STM32InternalFlashHalRaw::erase_page( std::size_t address, std::size_t size )
{
	const uint32_t start_offset = reinterpret_cast<uint32_t>(conf.data_ptr);
	std::size_t target_address = start_offset + address;
	return erase_page_by_page_startaddress(target_address, size );
}

} // namespace smt32_internal_flash


