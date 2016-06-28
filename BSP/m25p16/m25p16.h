#ifndef __M25P16_H__
#define __M25P16_H__
#include "stm32f10x.h"

#define FLASH_CHREAD 0x0B
#define FLASH_CLREAD 0x03
#define FLASH_PREAD	0xD2

#define FLASH_BUFWRITE1 0x84
#define FLASH_IDREAD 0x9F
#define FLASH_STATUS 0xD7
#define PAGE_ERASE 0x81
#define PAGE_READ 0xD2
#define MM_PAGE_TO_B1_XFER 0x53
#define BUFFER_2_WRITE 0x87
#define B2_TO_MM_PAGE_PROG_WITH_ERASE 0x86

#define BUFFER_1_WRITE 0x84
#define BUFFER_2_WRITE 0x87
#define BUFFER_1_READ 0xD4
#define BUFFER_2_READ 0xD6
#define B1_TO_MM_PAGE_PROG_WITH_ERASE 0x83
#define B2_TO_MM_PAGE_PROG_WITH_ERASE 0x86
#define MM_PAGE_TO_B1_XFER 0x53
#define MM_PAGE_TO_B2_XFER 0x55
#define PAGE_ERASE 0x81
#define SECTOR_ERASE 0x7C
#define READ_STATE_REGISTER 0xD7


/* Private typedef -----------------------------------------------------------*/
#define SPI_FLASH_PageSize    0x100

/* Private define ------------------------------------------------------------*/
#define WRITE      0x02  /* Write to Memory instruction */
#define WRSR       0x01  /* Write Status Register instruction */
#define WREN       0x06  /* Write enable instruction */

#define READ       0x03  /* Read from Memory instruction */
#define RDSR       0x05  /* Read Status Register instruction  */
#define RDID       0x9F  /* Read identification */
#define SE         0xD8  /* Sector Erase instruction */
#define BE         0xC7  /* Bulk Erase instruction */

#define WIP_Flag   0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte 0xA5

uint8_t spi_send_byte(uint8_t val);
uint8_t spi_recv_byte(void);
uint8_t spi_write_byte(uint8_t addr, uint8_t val);
uint8_t spi_read_byte(uint8_t addr);

void flash_page_erase(uint16_t page);
void flash_page_read(uint8_t *data, uint32_t addr, int length);
void flash_page_write(uint8_t *data, uint32_t addr, int length);
void flash_wait_busy(void);
uint8_t flash_check_id(void);

void flash_init(void);

#endif
