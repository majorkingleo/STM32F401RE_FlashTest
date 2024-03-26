/*
 * configuration file for STM32F401 prozessor
 *
 * Just defines symbols and sectors
 */
#ifndef APP_CPPUTILS_STM32F401_FLASH_CONFIG_H_
#define APP_CPPUTILS_STM32F401_FLASH_CONFIG_H_

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
 * At this example application sector 0 holds the vector table, so cannot be used
 * for writing. So we are starting with sector 1.
 *
 * Sector 4 has a size of 64k. The Raw driver cannot handle a configuration with different sector
 * sizes. Therefore we use then the JBOD driver.
 */

// the raw driver can only handle section with the same page size
static constexpr const stm32_internal_flash::Configuration::Sector fs_16k_sectors[] = {
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

static constexpr const stm32_internal_flash::Configuration::Sector fs_64k_sectors[] = {
  {
	FLASH_SECTOR_4,
	64*1024,
	ADDRESS_FLASH_SECTOR_4
  }
};

#endif /* APP_CPPUTILS_STM32F401_FLASH_CONFIG_H_ */
