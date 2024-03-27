/*
 * main_app.cc
 *
 *  Created on: Mar 1, 2024
 *      Author: Martin
 */
#include "main_app.h"
#include "SimpleOutDebug.h"
#include "stm32f401_flash_config.h"
#include <stm32f4xx_hal.h>
#include <array>
#include <span>
#include <format.h>
#include <string.h>
#include <memory>

#include <stm32_internal_flash_raw.h>
#include <GenericFlashDriver.h>
#include <JBODGenericFlashDriver.h>

using namespace Tools;

// pointer to flash space, automatically mapped by processor
volatile uint8_t flashFsData[112*1024] __attribute__ ((section(".flashfs_data")));

static const char MESSAGE1_INIT_NO_HAL[] { "Message 1, written before HAL init." };
static const char MESSAGE2[] { "Message 2, write accross sectors." };
static const unsigned MESSAGE2_OFFSET = 16*1024-10;

static const char MESSAGE3[] { "Message 3, write accross sectors with different size." };
static const unsigned MESSAGE3_OFFSET = 16*1024*3-10;

static const char MESSAGE4[] { "Message 4, write accross sectors with external buffer." };
static const unsigned MESSAGE4_OFFSET = 16*1024-10;

static std::array<std::byte,16*1024> external_buffer;
static std::span<std::byte> span_external_buffer = std::span<std::byte>(external_buffer);

namespace {
	std::span<const std::byte> to_span( const char* data ) {
		return std::span<const std::byte>(reinterpret_cast<const std::byte*>(data), strlen(data)+1);
	}

	std::string to_string( const std::span<std::byte> & data ) {
		return reinterpret_cast<const char*>(data.data());
	}
} // namespace


//////////// Test 1: Write to flash before HAL_Init //////////////

/**
 * This will be called before HAL_Init() was called,
 * to proof, that no system clock is needed.
 */
void test_write_message_no_hal_init_no_clock_init_1()
{
	using namespace stm32_internal_flash;

	Configuration conf;
	conf.used_sectors = flash_fs_16k_sectors;

	STM32InternalFlashHalRaw raw_driver( conf );
	GenericFlashDriver driver( raw_driver );

	driver.write(0, to_span(MESSAGE1_INIT_NO_HAL));
}

/**
 * This function is called in main, and reads and verifies the
 * message. For UART output HAL and sysclock is needed.
 */
void test_write_message_no_hal_init_no_clock_init_2()
{
	using namespace stm32_internal_flash;

	Configuration conf;
	conf.used_sectors = flash_fs_16k_sectors;

	STM32InternalFlashHalRaw raw_driver( conf );
	GenericFlashDriver driver( raw_driver );

	std::array<std::byte,50> buffer = {};
	std::span<std::byte> read_span(buffer);

	driver.read(0,read_span);
	std::string sread = to_string(read_span);

	CPPDEBUG( format("%s: \"%s\" => %s", __FUNCTION__, sread, sread == MESSAGE1_INIT_NO_HAL ? "Ok" : "ERROR" ));
}


void test_generic()
{
	using namespace stm32_internal_flash;

	Configuration conf;
	conf.used_sectors = flash_fs_16k_sectors;

	STM32InternalFlashHalRaw raw_driver( conf );
	GenericFlashDriver driver( raw_driver );

	driver.write(MESSAGE2_OFFSET, to_span(MESSAGE2));

	std::array<std::byte,50> buffer = {};
	std::span<std::byte> read_span(buffer);

	driver.read(MESSAGE2_OFFSET,read_span);
	std::string sread = to_string(read_span);

	CPPDEBUG( format("%s: \"%s\" => %s", __FUNCTION__, sread, sread == MESSAGE2 ? "Ok" : "ERROR" ));
}

void test_jbod()
{
	using namespace stm32_internal_flash;

	Configuration conf_16k;
	conf_16k.used_sectors = flash_fs_16k_sectors;
	STM32InternalFlashHalRaw raw_driver_16k( conf_16k );
	GenericFlashDriver driver_16k( raw_driver_16k );

	Configuration conf_64k;
	conf_64k.used_sectors = flash_fs_64k_sectors;
	STM32InternalFlashHalRaw raw_driver_64k( conf_64k );
	GenericFlashDriver driver_64k( raw_driver_64k );

	MemoryInterface* drivers_array[] = {
		&driver_16k,
		&driver_64k
	};


	JBODGenericFlashDriver driver( drivers_array );

	// We have not enough RAM to allocate a buffer of 64k,
	// so turn of restoring data. Now on write the whole page will be
	// erased.
	driver.set( JBODGenericFlashDriver::Property::RestoreDataOnUnaligendWrites(false) );
	driver.write(MESSAGE3_OFFSET, to_span(MESSAGE3));

	std::array<std::byte,100> buffer = {};
	std::span<std::byte> read_span(buffer);

	driver.read(MESSAGE3_OFFSET,read_span);
	std::string sread = to_string(read_span);

	CPPDEBUG( format("%s: \"%s\" => %s", __FUNCTION__, sread, sread == MESSAGE3 ? "Ok" : "ERROR" ));
}

void test_generic_external_buffer()
{
	using namespace stm32_internal_flash;

	Configuration conf;
	conf.used_sectors = flash_fs_16k_sectors;

	STM32InternalFlashHalRaw raw_driver( conf );
	GenericFlashDriver driver( raw_driver );

	// use this external buffer instead of allocating the buffer on stack
	driver.set( GenericFlashDriverBase::Property::PageBuffer{ &span_external_buffer } );

	driver.write(MESSAGE4_OFFSET, to_span(MESSAGE4));

	std::array<std::byte,100> buffer = {};
	std::span<std::byte> read_span(buffer);

	driver.read(MESSAGE4_OFFSET,read_span);
	std::string sread = to_string(read_span);

	CPPDEBUG( format("%s: \"%s\" => %s", __FUNCTION__, sread, sread == MESSAGE4 ? "Ok" : "ERROR" ));
}

void main_app()
{
	SimpleOutDebug out_debug;
	Tools::x_debug = &out_debug;

	CPPDEBUG( "start" );

	test_write_message_no_hal_init_no_clock_init_2();
	test_generic();
	test_jbod();
	test_generic_external_buffer();


	while( true ) {}
}

