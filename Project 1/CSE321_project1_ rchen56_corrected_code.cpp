 /* 
  * File: CSE321_project1_rchen56_corrected_code.cpp
  * 
  * Author: Ronald Chen (rchen56@buffalo.edu)
  * Date: Oct 4, 2021
  * Course: CSE321 
  * Required Materials: Nucleo L4R5ZI
  *
  * Inputs: BUTTON1, and LED2
  *
  * Outputs alternating 2000 ms on and 500 ms off time for blue LED
  * 
  *
  * Summary of File: 
  *             This program controls the blue LED has a thread drive the blue
  *             LED to have an on time of 2000ms and off time 500ms. 
  *   
  */ 
#include "mbed.h"


Thread controller;

void falling(); 
void rising();
void alternate(); 

InterruptIn interupt(BUTTON1);
DigitalOut blue(LED2); // 

int flag1= 0;
int flag2 = 0;


int main() {
    printf("----------------START----------------\n");
    printf("Starting state of thread: %d\n", controller.get_state());
    controller.start(alternate);
	printf("State of thread right after start: %d\n", controller.get_state());
    interupt.rise(rising);
	interupt.fall(falling);
    return 0;
}


 /** 
  * void alternate( ) 
  *
  * Parameters:     None 
  *
  * Return Value:   void
  * 
  * Description: 
  *             Alternates the signal of blue to be on for 2000 ms, and off 
  *             for 500 ms. 
  */ 
void alternate() {
    while (true) {
        if(flag1==0){
            blue = !blue;
            thread_sleep_for(2000); //Thread_sleep is a time delay function, causes a 2000 unit delay
            blue = !blue;
            thread_sleep_for(500); //Thread_sleep is a time delay function, causes a 500 unit delay
        }
    }
}

/** 
  * void rising() 
  *
  * Parameters:     None 
  *
  * Return Value:   void
  * 
  * Description: Flag2 = 1 when rising edge is detected.
  */ 									
void rising() {
	flag2=1;               
}

/** 
  * void sort( int array[] ) 
  *
  * Parameters:     None 
  *
  * Return Value:   void
  * 
  * Description: 
  *     falling() is called when there is a falling edge detected. 
  *     If flag2 == 1, 
  */ 
void falling() {
  if(flag2==1){	
    flag1++; 
    flag1 %= 2;
    flag2=0;
    }
}