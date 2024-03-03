/*
 * @author Copyright (c) 2024 Martin Oberzalek
 */
#include "stm32_internal_flash.h"
#include <stm32f4xx_hal.h>

namespace smt32_internal_flash {

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

bool STM32InternalFlashHal::erase_page( std::size_t address, std::size_t size )
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

void STM32InternalFlashHal::clear_flags()
{
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP |
			               FLASH_FLAG_OPERR |
						   FLASH_FLAG_WRPERR |
						   FLASH_FLAG_PGAERR |
						   FLASH_FLAG_PGSERR |
						   FLASH_FLAG_PGPERR |
						   FLASH_SR_RDERR |
						   FLASH_SR_BSY );
}

std::optional<Configuration::Sector> STM32InternalFlashHal::get_sector_from_address( std::size_t address ) const
{
	for( const Configuration::Sector & sec : conf.used_sectors ) {
		if( address == sec.start_address ) {
			return sec;
		}
	}

	return {};
}

std::size_t STM32InternalFlashHal::write_page( std::size_t address, const std::span<std::byte> & buffer )
{
	if( HAL_FLASH_Unlock() != HAL_OK) {
		error = Error(Error::ErrorUnlockingFlash);
		return 0;
	}

	AutoLockFlash auto_lock_flash;

	clear_flags();

	using data_t = uint32_t;
	constexpr uint32_t data_step_size = sizeof(data_t);
	std::size_t size_written = 0;

	for( uint32_t offset = 0; offset <= buffer.size() - data_step_size; offset += data_step_size ) {

		std::size_t target_address = address + offset;
		data_t *source = reinterpret_cast<data_t*>(buffer.data() + offset);
		uint64_t aligned_source_data = *source;

		HAL_StatusTypeDef ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
				target_address,
				aligned_source_data );

		if( ret != HAL_OK ) {
			error = Error(Error::HAL_Error,HAL_FLASH_GetError());
			return size_written;
		}

		size_written += data_step_size;
	}

	return size_written;
}

} // namespace smt32_internal_flash




