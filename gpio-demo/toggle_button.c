/*
 * toggle_button.c:
 ***********************************************************************
 *	WiringPi demo for GPIO basic control.
 *	Control	LED with toggle button. Interrupt Service Routine (ISR) is used
 *	to detect the changing of button's GPIO.
 *	Here are the features of wiringPi in this demo.
 *	- GPIO write/read
 *	- ISR of GPIO
 *
 ***********************************************************************
 */


#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>

#define LED_PIN		17
#define BUTTON_PIN	27

// global var for button count
uint32_t button_cnt = 0;

// callback function for GPIO interrupt
void buttonInterruptCallback();

int main(){
    // setput wiring-Pi GPIO mapping
    wiringPiSetup();

    // default setup of pin-mode & value for LED, button
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, 0);
    pinMode(BUTTON_PIN, OUTPUT);
    pullUpDnControl(BUTTON_PIN, PUD_UP);

    // set wiring-Pi GPIO interrupt
    wiringPiISR(BUTTON_PIN, INT_EDGE_FALLING, &buttonInterruptCallback);

    //main loop
    while(1){
	if(button_cnt % 2 == 0){
	    digitalWrite(LED_PIN, 0);
	}
	else{
	    digitalWrite(LED_PIN, 1);
	}		
    }
    return 0;
}

void buttonInterruptCallback(){
    button_cnt++;
    printf("button_cnt=%d\n", button_cnt);
}
