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
static const int MUTE_PIN = 17;
static const int BUS_SPEED = 500000;

// function prototype
uint8_t GainDataMapping(float gain_dB);

int main()
{
    int fd;
    
    // Setup wiring-Pi
    wiringPiSetup ();

    // Config IO for cs3310 control
    // - MUTE
    // - CS pin for SPI
    pinMode (MUTE_PIN, OUTPUT);
    pullUpDnControl(MUTE_PIN, PUD_DOWN); 
    pinMode (CS_PIN, OUTPUT);
    pullUpDnControl(CS_PIN, PUD_UP);
    digitalWrite(CS_PIN, 1);
    

    // Configure the SPI interface.
    printf("Initialize SPI...\n");
    fd = wiringPiSPISetup(CHANNEL, BUS_SPEED);
    printf("Init result:%d\n", fd);
    
    uint8_t data_buf[10];
    uint8_t data_r, data_l;

    float test_gain[5] = {0.0, 3.5, 6.0, 12.0, 20.0};

    while(1)
    {
        for(int i=0;i<5;i++)
        {
            // Write SPI data to CS3310
            data_r = GainDataMapping(test_gain[i]);
            data_l = GainDataMapping(test_gain[i]);
            data_buf[0] = data_l;
            data_buf[1] = data_r;
            digitalWrite(CS_PIN, 0);
            delay(10);
            wiringPiSPIDataRW(CHANNEL, data_buf, 2);
            digitalWrite(CS_PIN, 1);

            // Unmute CS3310
            digitalWrite(MUTE_PIN, 1);

	    delay(2000);
	}
    }
    return 0;
}

/* GainDataMapping:
 * description : generate binary code of CS3310 gain value coresponds to dB gain from the user
 * @arg : gain_dB (float)
 * @return : gain_bincode (uint8_t) 
 */
uint8_t GainDataMapping(float gain_dB)
{
    uint8_t gain_bincode;
    static float gain_per_bincode = 2.0;
    static int32_t bincode_offset = 192;

    gain_bincode = gain_per_bincode*gain_dB + bincode_offset;
    if(gain_bincode > 0xFF) 	gain_bincode = 0xFF;
    else if(gain_bincode < 0)	gain_bincode = 0;

    return gain_bincode; 	
}
