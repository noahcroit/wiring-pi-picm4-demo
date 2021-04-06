/******************************************************************************
spitest.cpp
Raspberry Pi SPI interface demo

Hardware connections:

The board was connected as follows:
(Raspberry Pi)(Serial 7 Segment)
GND  -> GND
3.3V -> Vcc
CE1  -> SS (Shift Select)
SCK  -> SCK 
MOSI -> SDI
MISO -> SDO

To build this file, I use the command:
>  g++ spitest.cpp -lwiringPi

Then to run it, first the spi kernel module needs to be loaded.  This can be 
done using the GPIO utility.
> gpio load spi
> ./a.out

******************************************************************************/

#include <errno.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>

// channel is the wiringPi name for the chip select (or chip enable) pin.
// Set this to 0 or 1, depending on how it's connected.
static const int CHANNEL = 0;
static const int CS_PIN = 5;
static const int BUS_SPEED = 500000;

uint8_t GainDataMapping(float gain_dB);

int main()
{
    int fd;
    
    // Setup wiring-Pi
    wiringPiSetup (); 

    // Configure the SPI interface.
    printf("Initialize SPI...\n");
    pinMode (CS_PIN, OUTPUT);
    pullUpDnControl(CS_PIN, PUD_UP);
    digitalWrite(CS_PIN, 1);
    fd = wiringPiSPISetup(CHANNEL, BUS_SPEED);
    printf("Init result:%d\n", fd);
    
    uint8_t data_buf[10];
    uint8_t data_r, data_l;
    data_r = GainDataMapping(0);
    data_l = GainDataMapping(0);
    data_buf[0] = data_l;
    data_buf[1] = data_r;

    wiringPiSPIDataRW(CHANNEL, data_buf, 2);
    delay(1000);

    return 0;
}

uint8_t GainDataMapping(float gain_dB)
{
    uint8_t gain_bincode;
    static int32_t gain_per_bincode = 2;
    static int32_t bincode_offset = 192;

    gain_bincode = gain_per_bincode*gain_dB + bincode_offset;
    if(gain_bincode > 0xFF) 	gain_bincode = 0xFF;
    else if(gain_bincode < 0)	gain_bincode = 0;

    return gain_bincode; 	
}
