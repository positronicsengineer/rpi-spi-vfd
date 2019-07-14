//Copyright (c) 2019 positronics.engineer at yandex dot com.
//Distributed under the terms of the MIT license.

/* Test SPI VFD display
 * Usage example for rpi-spi-vfd lib
 */
 
#include <stdio.h>
#include <unistd.h>
#include <bcm2835.h>
#include "rpi-spi-vfd.h"

int main(int argc, char** argv)
{
    const char *str1 = "Hello Pi";
    
    if (!bcm2835_init())
	{
		printf("bcm2835_init() failed. It must run by sudo!!!\n");
		return 1;
	}
    
    
    if (!vfd_set_spi())
    {
        printf("vfd_set_spi() failed!!!\n");
        return 1;
    }
    
    printf("Running VFD test...\n");
    vfd_init();
    
    //put string to screen
    vfd_put_str(str1);
    sleep(5);
    
    vfd_clear_screen();
    sleep(3);
    
    //put char
    vfd_put_char('R', 0);
    vfd_put_char('P', 1);
    vfd_put_char('i', 2);
    vfd_put_char(' ', 3);
    vfd_put_char(':', 4);
    vfd_put_char(')', 5);
    
    printf("Displaying test chars on VFD.\n" );
    sleep(5);
    
    vfd_set_dimming (200);
    printf("Increase VFD dimming to 200/255.\n" );
    sleep(3);
    
    //stby and back on
    printf("VFD goes to stand by mode and wakes up back.\n");
    vfd_stby(1);
    sleep(3);
    vfd_stby(0);
    sleep(3);
    
    vfd_clear_screen();
    vfd_stby(1);
    
    vfd_end();
    bcm2835_close();
    printf("Test finished.\n");
    return 0;
}
