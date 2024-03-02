/*
 * main_app.cc
 *
 *  Created on: Mar 1, 2024
 *      Author: Martin
 */
#include "main_app.h"
#include "SimpleOutDebug.h"
#include <stm32f4xx_hal.h>
#include <array>
#include <span>
#include <format.h>
#include <string.h>
#include <memory>

using namespace Tools;

volatile uint8_t flashFsData[48*1024] __attribute__ ((section(".flashfs_data")));

std::array<std::byte,16*1024> buffer;
extern uint32_t _flashfs_data_start;
extern uint32_t _flashfs_data_end;

void Write_Flash( const std::span<std::byte,16*1024> & buffer )
{
	FLASH_EraseInitTypeDef EraseInitStruct {};
	uint32_t PAGEError = 0;

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	//EraseInitStruct.Banks     = FLASH_BANK_1;
	EraseInitStruct.Sector	  = FLASH_SECTOR_1;
	EraseInitStruct.NbSectors = 1;
	EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3;

	HAL_FLASH_Unlock();

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP |
			               FLASH_FLAG_OPERR |
						   FLASH_FLAG_WRPERR |
						   FLASH_FLAG_PGAERR |
						   FLASH_FLAG_PGSERR |
						   FLASH_FLAG_PGPERR |
						   FLASH_SR_RDERR |
						   FLASH_SR_BSY );

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) {
		CPPDEBUG( "erasing flash page failed!");
		return;
	}

	using data_t = uint32_t;
	constexpr uint32_t data_step_size = sizeof(data_t);

	for( uint32_t offset = 0; offset <= buffer.size() - data_step_size; offset += data_step_size ) {
		std::byte *target_address = reinterpret_cast<std::byte*>(&_flashfs_data_start);
		target_address += offset;
		data_t *source = reinterpret_cast<data_t*>(buffer.data() + offset);
		uint64_t aligned_source_data = *source;
		HAL_StatusTypeDef ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
												  reinterpret_cast<uint32_t>(target_address),
												  aligned_source_data );

		if( ret != HAL_OK ) {

			uint32_t error = HAL_FLASH_GetError();

			if( error & HAL_FLASH_ERROR_RD ) {
				CPPDEBUG( "HAL_FLASH_ERROR_RD" );
			}
			if( error & HAL_FLASH_ERROR_PGS ) {
				CPPDEBUG( "HAL_FLASH_ERROR_PGS" );
			}
			if( error & HAL_FLASH_ERROR_PGP ) {
				CPPDEBUG( "HAL_FLASH_ERROR_PGP") ;
			}
			if(  error & HAL_FLASH_ERROR_PGA ) {
				CPPDEBUG( "HAL_FLASH_ERROR_PGA" );
			}
			if( error & HAL_FLASH_ERROR_WRP ) {
				CPPDEBUG( "HAL_FLASH_ERROR_WRP" );
			}
			if( error & HAL_FLASH_ERROR_OPERATION ) {
				CPPDEBUG( "HAL_FLASH_ERROR_OPERATION" );
			}

			CPPDEBUG( format( "writing flash page failed at offset: %d Error Code: 0x%X", offset, HAL_FLASH_GetError() ) );
			break;
		}

	}

	HAL_FLASH_Lock();
}

void main_app()
{
	SimpleOutDebug out_debug;
	Tools::x_debug = &out_debug;

	std::array<std::byte,16*1024> buffer{};
	snprintf( (char*)buffer.data(), buffer.size(), "%s", "Hello World" );

	void *ta = reinterpret_cast<std::byte*>(&_flashfs_data_start);
	std::byte *te = reinterpret_cast<std::byte*>(&_flashfs_data_end);
	CPPDEBUG( format( "target address: %p", ta ) );

	std::size_t space = _flashfs_data_end - _flashfs_data_start;
	std::byte *double_word_aligned = (std::byte*)std::align( alignof(uint64_t), sizeof(std::byte), ta, space );
	CPPDEBUG( format( "uint64_t aligned: %p", double_word_aligned ) );


	// CPPDEBUG( "writing data" );
	// Write_Flash( buffer );

	CPPDEBUG( format("reading data at: %p", (void*)flashFsData) );
	char acBuffer[100];
	memcpy( acBuffer, (void*)flashFsData, 100 );
	acBuffer[99]='\0';
	CPPDEBUG( acBuffer );

	while( true ) {}
}

