#include "m25p16.h"
#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "stdio.h"
#include "..\..\APP\includes.h"

#define NOSELECT()      GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define SELECT()        GPIO_ResetBits(GPIOB, GPIO_Pin_12)

void spi2_init(void)
{
    SPI_InitTypeDef spi;
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    //config spi2 sck miso mosi
    gpio.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &gpio);

    //config spi2 cs pin
    gpio.GPIO_Pin = GPIO_Pin_12;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &gpio);

    NOSELECT();

    spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_DataSize = SPI_DataSize_8b;
    spi.SPI_CPOL = SPI_CPOL_High;
    spi.SPI_CPHA = SPI_CPHA_2Edge;
    spi.SPI_NSS = SPI_NSS_Soft;
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    spi.SPI_FirstBit = SPI_FirstBit_MSB;
    spi.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &spi);

    SPI_Cmd(SPI2, ENABLE);
}

uint8_t spi_send_byte(uint8_t byte)
{
    SPI_I2S_SendData(SPI2, byte);

    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    /* Wait to receive a byte */
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI2);
}

uint8_t spi_recv_byte(void)
{
    return (spi_send_byte(0xA5));
}

uint8_t spi_write_byte(uint8_t addr, uint8_t val)
{
    //SELECT();
    //delay_ms(10);
    SPI_I2S_SendData(SPI2, addr << 8 | val);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    //delay_ms(10);
    //NOSELECT();

    return SPI_I2S_ReceiveData(SPI2) & 0xff;
}

uint8_t spi_read_byte(uint8_t addr)
{
    //SELECT();
    //delay_ms(10);
    SPI_I2S_SendData(SPI2, (addr | 0x80) << 8 | 0x00);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    //delay_ms(10);
    //NOSELECT();

    return SPI_I2S_ReceiveData(SPI2) & 0xff;
}

void flash_page_erase(uint16_t page)
{
    flash_wait_busy();
    SELECT();
    //write enable
    spi_send_byte(0x06);
    NOSELECT();

    flash_wait_busy();
    SELECT();
    //erase block
    spi_send_byte(SE);
    spi_send_byte((page & 0xff0000) >> 16);
    spi_send_byte((page & 0xff00) >> 8);
    spi_send_byte(page & 0xff);
    NOSELECT();
}

void flash_page_read(uint8_t *data, uint32_t addr, int length)
{
    uint32_t i;

    flash_wait_busy();
    SELECT();
    spi_send_byte(0x03);
    spi_send_byte((addr & 0xff0000) >> 16);
    spi_send_byte((addr & 0xff00) >> 8);
    spi_send_byte(addr & 0xff);

    for(i = 0; i < length; i++) {
        data[i] = spi_recv_byte();
    }
    NOSELECT();

}

void flash_page_write(uint8_t *data, uint32_t addr, int length)
{
    uint32_t i;

    flash_page_erase(0x01);

    flash_wait_busy();
    SELECT();
    spi_send_byte(0x06);
    NOSELECT();

    flash_wait_busy();
    SELECT();
    spi_send_byte(0x02);
    spi_send_byte((addr & 0xff0000) >> 16);
    spi_send_byte((addr & 0xff00) >> 8);
    spi_send_byte(addr & 0xff);
    for(i = 0; i < length; i++) {
        spi_send_byte(data[i]);
    }

    NOSELECT();
}

void flash_wait_busy(void)
{
    uint8_t status = 0;
    SELECT();
    spi_send_byte(RDSR);
    do {
        status = spi_send_byte(Dummy_Byte);
    } while((status & WIP_Flag) == SET);
    NOSELECT();
}

uint8_t flash_check_id(void)
{
    uint8_t id[4];
    SELECT();
    spi_send_byte(RDID);
    id[0] = spi_recv_byte();
    id[1] = spi_recv_byte();
    id[2] = spi_recv_byte();
    NOSELECT();
    printk("id = 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
            id[0], id[1], id[2], id[3]);
    if(id[0] == 0x20 && (id[1] == 0x20)
            && (id[2] == 0x15)) {
        return 0;
    } else {
        return 0xff;
    }
}

void flash_init(void)
{
    //uint8_t temp;
    //uint8_t buffer[128];

    spi2_init();
    OSTimeDly(OS_TICKS_PER_SEC / 100);
    if(!flash_check_id()) {
        printk("flash id check successfully!\r\n");
        /*flash_page_read(buffer, 0, 16);
          printk("buffer: %s", buffer);
          sprintf((char *)buffer, "hello, world!!\r\n");
          flash_page_write(buffer, 0, 16);
          flash_page_read(buffer, 0, 16);
          printk("buffer: %s", buffer);
          */
    } else {
        printk("flash id check failed\r\n");
    }
}
