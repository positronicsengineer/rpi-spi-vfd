//Copyright (c) 2019 positronics.engineer at yandex dot com.
//Distributed under the terms of the MIT license.

/* A simple and basic driver for Futaba 8-MD-06INKM C2CIG type VFD display for Raspberry Pi.
 */
 
#include <stdio.h>
#include <unistd.h>
#include <bcm2835.h>
#include <string.h>

#include "rpi-spi-vfd.h"

#define DATA_LEN 2 //each command 2 bytes 

/* VFD device needs LSB first out on SPI bus. 
 * This s not possible by BCM2835 SPI module hardware, use soft function to reverse order the byte data
 * First the left four bits are swapped with the right four bits. 
 * Then all adjacent pairs are swapped and then all adjacent single bits. 
 * This results in a reversed order.
 * https://stackoverflow.com/a/2602885
 */
uint8_t msb2lsb_revord(uint8_t msb)
{
    msb = (msb & 0xF0) >> 4 | (msb & 0x0F) << 4;
    msb = (msb & 0xCC) >> 2 | (msb & 0x33) << 2;
    msb = (msb & 0xAA) >> 1 | (msb & 0x55) << 1;
    return msb;
}

int vfd_set_spi(void)
{
	if (!bcm2835_spi_begin())
	{
		printf("bcm2835_spi_begin() failed!!!\n");
		return 0;
	}
	
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_LSBFIRST); //This is important but it is fake in bcm2835 lib, need to use MSB to LSB reverse order function
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);
    bcm2835_spi_setClockDivider(SPI_CLK_FREQ);
    bcm2835_spi_chipSelect(SPI_CS_PIN);
    bcm2835_spi_setChipSelectPolarity(SPI_CS_PIN, LOW);

    // Set VFD_RST_PIN to be an output, once
	bcm2835_gpio_fsel(VFD_RST_PIN, BCM2835_GPIO_FSEL_OUTP);    
 
	return 1;
}

void vfd_rst(void)
{
    bcm2835_gpio_write(VFD_RST_PIN, LOW);
    usleep(RST_DELAY);
    bcm2835_gpio_write(VFD_RST_PIN, HIGH);
}

//Follow flow chart Fig 4-1-1 on page17 in spec
void vfd_init(void)
{
    char wbuf[DATA_LEN], rbuf[DATA_LEN];
    memset(rbuf, 0, sizeof(rbuf)); 
    memset(wbuf, 0, sizeof(wbuf));
    vfd_rst();
    //set display timing
    wbuf[0] = msb2lsb_revord(0xE0);
    wbuf[1] = msb2lsb_revord(0x07); //0x07 for 8-MD-06INKM
    bcm2835_spi_transfernb(wbuf, rbuf, DATA_LEN); //don't care read data 
    usleep(CMD_DELAY);
    //set dimming, default 50/255
    vfd_set_dimming (50);
    //display light up
    wbuf[0] = msb2lsb_revord(0xE8);
    //wbuf[1] = msb2lsb_revord(0x00); //second byte don't care
    bcm2835_spi_transfernb(wbuf, rbuf, DATA_LEN); 
    usleep(CMD_DELAY);
}

void vfd_set_dimming (const uint8_t dimm)
{
    char wbuf[DATA_LEN], rbuf[DATA_LEN];
    memset(rbuf, 0, sizeof(rbuf)); 
    memset(wbuf, 0, sizeof(wbuf));
    wbuf[0] = msb2lsb_revord(0xE4);
    wbuf[1] = msb2lsb_revord(dimm);
    bcm2835_spi_transfernb(wbuf, rbuf, DATA_LEN); //don't care read data 
    usleep(CMD_DELAY);
}

void vfd_stby (const uint8_t mode)
{
    char wbuf[DATA_LEN], rbuf[DATA_LEN];
    memset(rbuf, 0, sizeof(rbuf)); 
    memset(wbuf, 0, sizeof(wbuf));
    if ((mode == 1)|(mode == 0))
    {
        wbuf[0] = msb2lsb_revord(0xEC|mode);
        //wbuf[1] = 0x00; //2nd byte don't care
        bcm2835_spi_transfernb(wbuf, rbuf, DATA_LEN); //don't care read data 
        usleep(CMD_DELAY);
    }
    else
        printf("invalid mode for stby command, set 0 or 1\n");
}

void vfd_end (void)
{
    bcm2835_spi_end();
}

void vfd_clear_screen(void)
{
    const char *blnk = "        ";
    vfd_put_str(blnk);
}

void vfd_put_char (const char ch, const uint8_t digit)
{
    char wbuf[DATA_LEN], rbuf[DATA_LEN];
    memset(rbuf, 0, sizeof(rbuf)); 
    memset(wbuf, 0, sizeof(wbuf));
    if(digit < VFD_WIDTH)
    {
        wbuf[0] = msb2lsb_revord((0b00100000|digit)); //assume digit equal or less than 0b00000111
        wbuf[1] = msb2lsb_revord(ch);
        bcm2835_spi_transfernb(wbuf, rbuf, DATA_LEN); //don't care read data 
        usleep(CMD_DELAY);
    }
    else
        printf("Digit parameter is greater than 8-MD-06INKM VFD spec!!!\n");
}

void vfd_put_str(const char *str)
{
	uint8_t i = 0; 
    if (strlen(str) <= VFD_WIDTH)
	{
		while (*str)
		{
			vfd_put_char(*str, i);
			str++;
            i++;
		}
	}
	else
		printf("Number of chars in string is greater than screen!!!\n");
}
