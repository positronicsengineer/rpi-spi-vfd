//Copyright (c) 2019 positronics.engineer at yandex dot com.
//Distributed under the terms of the MIT license.

/* Description:
 * This is a simple and basic driver for Futaba 8-MD-06INKM C2CIG type VFD display for Raspberry Pi.
 * Device has 1 line, 8 digits and build in character controller, each digit is 5x7 dot matrix format.
 * https://www.futaba.co.jp/en/display/vfd/lineup.html
 *
 * It was developed and tested on Raspberry Pi model B.
 * Based on the bcm2835 C library.
 * http://www.airspayce.com/mikem/bcm2835/
 *
 * References:
 * http://www.airspayce.com/mikem/bcm2835/spi_8c-example.html
 * https://github.com/CrosseyeJack/spincl
 *
 * Change notes:
 * Inital version, June-2019
 */

#ifndef RPI_SPI_VFD_H
#define RPI_SPI_VFD_H

//Device parameters
#define VFD_LINE 1 //Number of lines
#define VFD_WIDTH 8 //Maximum characters per line

//Timing constants
#define CMD_DELAY 1 //us
#define RST_DELAY 15 //us

//SPI parameters
#define SPI_CLK_FREQ BCM2835_SPI_CLOCK_DIVIDER_16384 //15.25878906kHz on Rpi2, device spec max 500kHz
#define SPI_CS_PIN BCM2835_SPI_CS1 //Pin26 CE1#
#define VFD_RST_PIN RPI_GPIO_P1_22 //Pin#X of header P1

/* must be first called function
 * sets up SPI bus etc
 */
int vfd_set_spi(void);

/* must be second called function
 */
void vfd_init (void);

/* returns SPI bus to GPIO frunction
 */
void vfd_end (void);

/* Hard reset to VFD by defined GPIO
 */
void vfd_rst(void);

/* clears all data on screen
 * actually puts " " (space char) on each digit
 */
void vfd_clear_screen (void);

/* send string to vfd
 * length of string must be less than VFD_WIDTH
 */
void vfd_put_str (const char *str);

/* send a single char to given vfd digit, digit must be less than VFD_WIDTH
 */
void vfd_put_char (const char ch, const uint8_t digit);

/* stby mode
 * 1 go to stby mode
 * 0 normal mode
 * screen data is not effected, just lights off
 */
void vfd_stby (const uint8_t mode);

/* set vfd brightness (dimming, duty cycle)
 * vfd_init() sets it as 50/255 by default
 * device limits max as 240/255
 */
void vfd_set_dimming (const uint8_t dimm);

#endif //RPI_SPI_VFD_H
