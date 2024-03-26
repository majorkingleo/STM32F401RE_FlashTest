/*
 * Symbols and definitions for STM32F401
 * @author Copyright (c) 2024 Martin Oberzalek
 */


#ifndef APP_STM32F401_FLASH_CONFIG_H_
#define APP_STM32F401_FLASH_CONFIG_H_

#include <stm32_internal_flash.h>

/*
 * Sector 0 0x0800 0000 - 0x0800 3FFF 16 Kbyte
 * Sector 1 0x0800 4000 - 0x0800 7FFF 16 Kbyte
 * Sector 2 0x0800 8000 - 0x0800 BFFF 16 Kbyte
 * Sector 3 0x0800 C000 - 0x0800 FFFF 16 Kbyte
 * Sector 4 0x0801 0000 - 0x0801 FFFF 64 Kbyte
 * Sector 5 0x0802 0000 - 0x0803 FFFF 128 Kbyte
 * Sector 6 0x0804 0000 - 0x0805 FFFF 128 Kbyte
 * .
 * .
 * .
 * Sector 11 0x080E 0000 - 0x080F FFFF 128 Kbyte
 */
#define ADDRESS_FLASH_SECTOR_0   0x08000000
#define ADDRESS_FLASH_SECTOR_1   (ADDRESS_FLASH_SECTOR_0  +  16*1024)
#define ADDRESS_FLASH_SECTOR_2   (ADDRESS_FLASH_SECTOR_1  +  16*1024)
#define ADDRESS_FLASH_SECTOR_3   (ADDRESS_FLASH_SECTOR_2  +  16*1024)
#define ADDRESS_FLASH_SECTOR_4   (ADDRESS_FLASH_SECTOR_3  +  16*1024)
#define ADDRESS_FLASH_SECTOR_5   (ADDRESS_FLASH_SECTOR_4  +  64*1024)
#define ADDRESS_FLASH_SECTOR_6   (ADDRESS_FLASH_SECTOR_5  + 128*1024)
#define ADDRESS_FLASH_SECTOR_7   (ADDRESS_FLASH_SECTOR_6  + 128*1024)
#define ADDRESS_FLASH_SECTOR_8   (ADDRESS_FLASH_SECTOR_7  + 128*1024)
#define ADDRESS_FLASH_SECTOR_9   (ADDRESS_FLASH_SECTOR_8  + 128*1024)
#define ADDRESS_FLASH_SECTOR_10  (ADDRESS_FLASH_SECTOR_9  + 128*1024)
#define ADDRESS_FLASH_SECTOR_11  (ADDRESS_FLASH_SECTOR_10 + 128*1024)

/**
 * The raw driver cannot handle different sections sizes at once
 */

// the raw driver can only sections with the same page size
static constexpr const stm32_internal_flash::Configuration::Sector flash_fs_16k_sectors[] = {
  {
	FLASH_SECTOR_1,
	16*1024,
	ADDRESS_FLASH_SECTOR_1
  },
  {
	FLASH_SECTOR_2,
	16*1024,
	ADDRESS_FLASH_SECTOR_2
  },
  {
	FLASH_SECTOR_3,
	16*1024,
	ADDRESS_FLASH_SECTOR_3
  },
};

static constexpr const stm32_internal_flash::Configuration::Sector flash_fs_64k_sectors[] = {
  {
	FLASH_SECTOR_4,
	64*1024,
	ADDRESS_FLASH_SECTOR_4
  }
};


#endif /* APP_STM32F401_FLASH_CONFIG_H_ */
