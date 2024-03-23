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

#include <stm32_internal_flash_raw.h>
#include <GenericFlashDriver.h>

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


/*
 * Flash module organization (STM32F401xB/C and STM32F401xD/E)
 * Block Name Block base addresses Size
 *
 * Sector 0 0x0800 0000 - 0x0800 3FFF 16 Kbytes
 * Sector 1 0x0800 4000 - 0x0800 7FFF 16 Kbytes
 * Sector 2 0x0800 8000 - 0x0800 BFFF 16 Kbytes
 * Sector 3 0x0800 C000 - 0x0800 FFFF 16 Kbytes
 * Sector 4 0x0801 0000 - 0x0801 FFFF 64 Kbytes
 * Sector 5 0x0802 0000 - 0x0803 FFFF 128 Kbytes
 * Sector 6 0x0804 0000 - 0x0805 FFFF 128 Kbytes
 * Sector 7 0x0806 0000 - 0x0807 FFFF 128 Kbytes
 */

void test_internal_flash_driver_raw()
{
	using namespace stm32_internal_flash;

	Configuration::Sector sectors[] = {
	  {
		FLASH_SECTOR_1,
		16*1024,
		0x08004000
	  },
	  {
		FLASH_SECTOR_2,
		16*1024,
		0x08008000
	  },
	  {
		FLASH_SECTOR_3,
		16*1024,
		0x0800C000
	  },
	};

	Configuration conf;
	conf.data_ptr = reinterpret_cast<std::byte*>(&_flashfs_data_start);
	conf.used_sectors = sectors;

	static std::array<std::byte,16*1024> buffer{};
	snprintf( (char*)buffer.data(), buffer.size(), "%s", "Hello World2" );

	STM32InternalFlashHalRaw raw_driver( conf );

	if( !raw_driver ) {
		CPPDEBUG( "initializing raw driver failed" );
		return;
	}

	CPPDEBUG( "erasing" );
	if( !raw_driver.erase_page_by_page_startaddress( sectors[0].start_address, sectors[0].size ) ) {
		CPPDEBUG( "erasing page failed" );
		return;
	}

	CPPDEBUG( "writing" );
	std::size_t data_written = raw_driver.write_page(0, buffer);
	if( data_written != buffer.size() ) {
		CPPDEBUG( "writing page failed" );
		return;
	}

	CPPDEBUG( "reading" );
	static std::array<std::byte,100> read_buffer{};
	std::span<std::byte> sread_buffer( read_buffer );
	raw_driver.read_page( 0, sread_buffer );

	CPPDEBUG( format("reading data: '%s'", (char*)read_buffer.data()) );
}

void test_internal_flash_driver_generic()
{
	using namespace stm32_internal_flash;

	Configuration::Sector sectors[] = {
	  {
		FLASH_SECTOR_1,
		16*1024,
		0x08004000
	  },
	  {
		FLASH_SECTOR_2,
		16*1024,
		0x08008000
	  },
	  {
		FLASH_SECTOR_3,
		16*1024,
		0x0800C000
	  },
	};

	Configuration conf;
	conf.data_ptr = reinterpret_cast<std::byte*>(&_flashfs_data_start);
	conf.used_sectors = sectors;

	STM32InternalFlashHalRaw raw_driver( conf );
	GenericFlashDriver driver( raw_driver );

	static std::array<std::byte,100> read_buffer{};
	std::span<std::byte> sread_buffer( read_buffer );
	std::size_t len = 0;

#if 0
	char buffer[] { "Hello World3. This is Cool! Stuff." };
	std::span<std::byte> span_buffer( (std::byte*)buffer, std::size(buffer) );

	CPPDEBUG( "writing" );
	len = driver.write(0, span_buffer);
	if( len != span_buffer.size() ) {
		CPPDEBUG( format( "writing data failed. Len: %d", len )  );
		return;
	}

	CPPDEBUG( "reading" );
	len = driver.read( 0, sread_buffer );
	if( len != sread_buffer.size() ) {
		CPPDEBUG( "reading failed" );
		return;
	}

	CPPDEBUG( format("reading data: '%s'", (char*)read_buffer.data()) );
#endif
#if 0
	std::size_t address = driver.get_page_size() * 2 - 10;


	char buffer2[] { "Hello World4. This is Cool! Stuff. YYY3" };
	std::span<std::byte> span_buffer2( (std::byte*)buffer2, std::size(buffer2) );

	CPPDEBUG( "writing" );
	len = driver.write(address, span_buffer2);
	if( len != span_buffer2.size() ) {
		CPPDEBUG( format( "writing data failed. Len: %d", len )  );
		return;
	}


	CPPDEBUG( "reading" );
	strcpy( (char*)sread_buffer.data(), "XXXXXXXXXXXXXXXXXXXX" );
	len = driver.read( address, sread_buffer );
	if( len != sread_buffer.size() ) {
		CPPDEBUG( "reading failed" );
		return;
	}


	CPPDEBUG( format("reading data: '%s'", (char*)read_buffer.data()) );
#endif

	std::size_t address = driver.get_page_size() - 10;
	std::vector<std::byte> big_buffer(16*1024+100);

	strcpy( (char*)&big_buffer[0], "Test4XXXXXXXXXXXXXXXXXXXXYYYYYYYYYYYYYYYYY" );
	std::string test_text = "Test5ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ1";
	const unsigned text_buffer_offset = big_buffer.size()-test_text.size()-1;
	strcpy( (char*)&big_buffer[text_buffer_offset], test_text.c_str()  );
	std::span<std::byte> span_buffer3( &big_buffer[0], big_buffer.size() );

	CPPDEBUG( "writing" );
	len = driver.write(address, span_buffer3);
	if( len != span_buffer3.size() ) {
		CPPDEBUG( format( "writing data failed. Len: %d", len )  );
		return;
	}


	CPPDEBUG( "reading" );
	strcpy( (char*)sread_buffer.data(), "XXXXXXXXXXXXXXXXXXXX" );

	len = driver.read( address, sread_buffer );
	if( len != sread_buffer.size() ) {
		CPPDEBUG( "reading failed" );
		return;
	}

	CPPDEBUG( format("reading data: '%s'", (char*)read_buffer.data()) );
	std::span<std::byte> sread_buffer2 = sread_buffer.subspan(0, test_text.size());

	len = driver.read( address + text_buffer_offset, sread_buffer2 );
	if( len != sread_buffer2.size() ) {
		CPPDEBUG( "reading failed" );
		return;
	}

	CPPDEBUG( format("reading data: '%s'", (char*)read_buffer.data()) );
}

void main_app()
{
	SimpleOutDebug out_debug;
	Tools::x_debug = &out_debug;

	CPPDEBUG( "start" );

	std::array<std::byte,16*1024> buffer{};
	snprintf( (char*)buffer.data(), buffer.size(), "%s", "Hello World" );

#if 0
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
#endif

//	test_internal_flash_driver_raw();
	test_internal_flash_driver_generic();


	while( true ) {}
}

