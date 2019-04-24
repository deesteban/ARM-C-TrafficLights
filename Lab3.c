#include <stm32f10x_cl.h>
#include "DspCode.h"

#define DELAY_C 0x0 // this needs to be tried out to finalize 

#define North 0x0100
#define NorthA 0x0200
#define South 0x0400
#define SouthA 0x0800
#define West 0x1000
#define WestA 0x2000
#define East 0x4000
#define EastA 0x8000


enum light_state {NS,EW};

enum light_state state;

int joystick;
int j;
void SystemInit(){}

// this function read the joystick and remember if the joysticks are ever selected.
// this function can also be used as a delay function. The length of the delay is determined by 
// delay_count.  This function can be called for one time unit delay.

int Read_joystick(){
int stick;
stick = joystick;
stick |=~GPIOD->IDR; //read joystick each loop
stick &= 0xf000; //relevant bits
return stick;
}

void Welcome()
{
GLCD_Init(); /* Initialize graphical LCD display */
GLCD_Clear(White); /* Clear graphical LCD display */
GLCD_SetTextColor(White);
GLCD_SetBackColor(Black);
GPIOE->ODR = 0xff00; //For timing
GLCD_DisplayString(0, 0, "---Traffic Light---");
GLCD_DisplayString(1, 0, "-----Simulator----- ");
GLCD_DisplayString(3, 0, " Simulates ");
GLCD_DisplayString(4, 0, " North-South and ");
GLCD_DisplayString(5, 0, " East-West ");
GLCD_DisplayString(6, 0, " Intersection. ");
GPIOE->ODR = 0x0000; //timing
}

void waitF(int nOfUnits){
 int countWait=nOfUnits;
	countWait*=DELAY_C;
	
	while(countWait!=0){
		joystick=Read_joystick();
		countWait-=0x1;
	}
}

	
int main (void) 
{
int WS,EN,SE,NW,joystick2,mainDelay;
state = NS;
RCC->APB2ENR |= 1 << 6; // Enable GPIOE clock
RCC->APB2ENR |= 1 << 5; //Enable GPIOD clock
GPIOE->CRH = 0x33333333; //Configure the GPIO for LEDs
//GPIOD->CRH = 0x00000000; //Configure the GPIO for Joystick - Input Analog
joystick =0;  // this is used to collect the joystick reading, It remembers all right turn requests
                        // initialized to 0, ie no right turning vehicles at all sides.
mainDelay=1;
WS=0,EN=0,SE=0,NW=0;
Welcome();
for (;;) {
   switch (state)
                  {
                   case NS : 
											
	// all lights to be Red for one time unit 
	// use the Read_joystick function to read the joystick and also delay 
// one time unit
                        GPIOE->ODR = 0;  // all red
												waitF(1);
												mainDelay=10;
												if(SE==1&&NW==1){
														//both right arrows are lit and straight lights are off for 3 TU.
														GPIOE->ODR =  NorthA|SouthA;
														GLCD_DisplayString(8, 0, "South-Right & North-Right Green.");
														waitF(3);
														//all red 1 TU
														GPIOE->ODR = 0;
														waitF(1);
														//set mainDelay to 6 for NS traffic
														mainDelay=6;
														SE=0;
														NW=0;
														joystick&=0xA000;
												}else if (SE==1&&NW==0){
															//straight and right arrow green.
														GPIOE->ODR =  SouthA|South;
														GLCD_DisplayString(8, 0, "South-Right & South-North Green.");
														waitF(3);
														//right arrow red, straight continues green while right lane is cleared.
														GPIOE->ODR = South;
														waitF(1);
														//set mainDelay to 6 for NS traffic
														mainDelay=6;
														SE=0;
														joystick&=0xB000;
												}else if (NW==1&&SE==0){
																//straight and right arrow green.
														GPIOE->ODR =  NorthA|North;
														GLCD_DisplayString(8, 0, "North-Right & North-South Green.");
														waitF(3);
														//right arrow red, straight continues green while right lane is cleared.
														GPIOE->ODR = North;
														waitF(1);
														//set mainDelay to 6 for NS traffic
														mainDelay=6;
														NW=0;
														joystick&=0xE000;
												}
											 
														// SN straight green for 10 time units
														GPIOE->ODR =  North|South; // Turn on LEDs on N  and S 
														GLCD_DisplayString(8, 0, "North & South Green.      ");
														waitF(mainDelay);
														//we check if any cars are waiting on the West side wanting to go right.
														joystick2=joystick;
														joystick2&=0x2000;
														if(joystick2==0x2000){
															GLCD_DisplayString(9, 0, "West Turn Requested      ");
															WS=1;
														}
														//we check for cars on the East wanting to go right.
														joystick2=joystick;
														joystick2&=0x8000;
														if(joystick2==0x8000){
															GLCD_DisplayString(9, 0, "East Turn Requested      ");
															EN=1;
														}
												
														state = EW;  // next state is EW

										break;
                  case EW : 

									
									 GPIOE->ODR = 0;  // all red
												waitF(1);
												mainDelay=10;
												if(WS==1&&EN==1){
														//both right arrows are lit and straight lights are off for 3 TU.
														GPIOE->ODR =  WestA|EastA;
														
														GLCD_DisplayString(8, 0, "East-Right & West-Right Green.");
														waitF(3);
														//all red 1 TU
														GPIOE->ODR = 0;
														waitF(1);
														//set mainDelay to 6 for NS traffic
														mainDelay=6;
														WS=0;
														EN=0;
														joystick&=0x5000;
												}else if (WS==1&&EN==0){
															//straight and right arrow green.
														GPIOE->ODR =  WestA|West;
														GLCD_DisplayString(8, 0, "West-Right & West-East Green. ");
														waitF(3);
														//right arrow red, straight continues green while right lane is cleared.
														GPIOE->ODR = West;
														
														waitF(1);
														//set mainDelay to 6 for NS traffic
														mainDelay=6;
														WS=0;
														joystick&=0xD000;
												}else if (EN==1&&WS==0){
																//straight and right arrow green.
														GPIOE->ODR =  EastA|East;
														GLCD_DisplayString(8, 0, "East-Right & East-West Green. ");
														waitF(3);
														//right arrow red, straight continues green while right lane is cleared.
														GPIOE->ODR = East;
														waitF(1);
														//set mainDelay to 6 for NS traffic
														mainDelay=6;
														EN=0;
														joystick&=0x7000;
												}
											 
														// EW straight green for 10 time units
														GPIOE->ODR =  East|West; // Turn on LEDs on E  and W
														GLCD_DisplayString(8, 0, "East& West Green. ");
														waitF(mainDelay);
														//we check if any cars are waiting on the South side wanting to go right.
														joystick2=joystick;
														joystick2&=0x4000;
														if(joystick2==0x4000){
															SE=1;
															GLCD_DisplayString(9, 0, "South Turn Requested.      ");
														}
														//we check for cars on the North wanting to go right.
														joystick2=joystick;
														joystick2&=0x1000;
														if(joystick2==0x1000){
															NW=1;
															GLCD_DisplayString(9, 0, "North Turn Requested.       ");
														}
												
														state = NS;  // next state is NS
									
										break;
														
								  default : 
												GPIOE->ODR = 0x0000;
		  									GLCD_DisplayString(8, 0,"There is a problem! ");
												waitF(5);
												state =NS; 
										break;
                 }
        }
}


