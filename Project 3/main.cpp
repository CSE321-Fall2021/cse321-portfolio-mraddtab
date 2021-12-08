
/* 
Author: Ronald Chen 
Course: CSE321
Creation Date: 12/5/2021
Program Name: Humidity Alarm
Program Summary: This program is the software required to operate the humidity alarm being designed. 
                 The humidity alarm is a device that detects the humidity level in a room, and alarms 
                 the user when the humidity percentage becomes to high or low via the buzzer peripheral.
                 The buzzer will continue to output noise until levels return to normal or the button B1
                 on the Nucleo is pressed. The humidity percentage and saftey level will be printed onto 
                 the LCD. 
Inputs: DHT11 humidity data, User button B1
Outputs: Humidity percentage, and saftey level printed to LCD, Buzzer audio output.
Implementation Requirements: Watchdog timer, Synchronization technique, Bitwise driver configuration,
                             Critical section protection, Use of threads/tasks, 1 Interrupt.
Required Materials: Nucleo-L4R5ZI, DHT11 Sensor, Buzzer audio module, 16x2 LCD Display, Wires, Resistors,
                    Breadboard.
Additional Info: The buzzer is activated on an actived on an active low signal. 
                 Sampling period of DHT11 must be > 2 seconds.
                 Pin Mappings:SCL: PB_8, SDA: PB_9, Buzzer: PB_15, Button 1: PC_13, DHT11: PC_6
                 
*/
#include "mbed.h"
#include "DHT11.h"
#include "1802.h"
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <string>
#include <stdio.h>


int sense_interval = 3000;              // DHT11 Senses every 2.5 seconds.
const uint32_t watchdog_timeout = 5000; // Watchdog times out every 3.0 seconds.        
//Threads and Interrupts.
Thread t_sense;                         // This thread will sense humidity.
void sense();
Thread t_display;                       // This thread will update the LCD
void output();
Thread t_audio;                         // This thread will control the buzzer.
void audio();

InterruptIn mute_pressed(PC_13);        // User Button is the mute button.
void mute();

DHT11 sensor(PC_6);
CSE321_LCD LCD(16,2,LCD_5x10DOTS,PB_9,PB_8);

Queue<int,32> humidityQue;
Queue<string,32> stateQue1;
bool muted = false;
string state;
int humidity;
Mutex mux;
int main()
{
    LCD.begin();
    RCC->AHB2ENR |= 0x6;            //Enable ports B and C.

    GPIOB->MODER &=~(0x800A0000);   //Configure GPIOB Moder: Output (01) for pins 8(SCL), 9(SDA) & 15(Buzzer).
    GPIOB->MODER |= 0x40050000;

    GPIOC->MODER &=~(0xC003000);    //Configure GPIOC Moder: Input (00) for pins 6(DHT) & 13(User Button).
    GPIOB->ODR |= 0x8000;           //Enable output for LCD pins 8 & 9, and disable buzzer noise pin 15.
    GPIOC->IDR |= 0x40;             //Configure DHT11 IDR: PC_6

    // Watchdog is configured to be greater than the time it should required to
    // read data from the sensor, and update the LCD.
    Watchdog &watchdog = Watchdog::get_instance();  
    if(watchdog.start(watchdog_timeout) == true && watchdog.is_running() == true){
        printf("Watchdog timer started sucessfully \nTimeout initialized to: %ims\n", watchdog.get_timeout());
    }
    else{
        printf("Watchdog timer failed to start.\n");
    }

    t_sense.start(sense);  
    t_display.start(output);
    mute_pressed.rise(mute);
    
}

/*
Function Name: sense()
Function Purpose: This function is executed by the thread t_sense,
                  and will continuouslly update the global variables
                  "humidity" and "state". Saftey Levels: 
                    Ideal: 30% < Humidity% < 60%
                    Fair: 25% <= Humidity% <= 30% OR 60% <= humidity% <=70%.
                    Harmful: 30% > humidity% > 70%, 
Inputs: Data from DHT11 Sensor
Outputs: Sends &humidity to humidityQue, and &state to stateQue.
Constraints: Sensor can only sense in intervals greater than 2 seconds.
*/
void sense(){
    while(true){
        thread_sleep_for(sense_interval);
        string state;
        sensor.read();
        humidity = sensor.getHumidity();
        if(humidity < 30 || humidity < 60 ){
            state = "Ideal";
        }
        else if ((humidity >= 25 && humidity <= 30) || (humidity >= 60 && humidity <= 70)) {
            state = "Fair";
        }
        else if(humidity < 30 || humidity > 70){
            state = "Harmful";
        }
        stateQue1.try_put(&state);
        humidityQue.try_put(&humidity);
    }
}

/*
Function Name: output()
Function Purpose: This function will wait for humidity and state 
                  data from the two Queues and display the humidity 
                  and state on the LCD display as such:
                  Humidity Percentage: __ %
                  Saftey Level: "".
                  
Inputs: int humidity, string level.
Outputs: Update LCD everytime data is given to the queue changes.
         Activates buzzer if state == "Harmful", and disables it
         otherwise.
Constraints: Can only update as fast as sensor is able to sense.
 */
void output(){ 
        while(true){
            osEvent humidityMsg = humidityQue.get(osWaitForever);
            osEvent stateMsg = stateQue1.get(osWaitForever);
            LCD.clear();
            state = *(string*)stateMsg.value.p;
            humidity = *(int*)humidityMsg.value.p;
            string humidity_str = "Humidity: "+ std::to_string(*(int*)humidityMsg.value.p) + "%";
            string state_str = "State: " + *(string*)stateMsg.value.p;
            const char * humidity_const = humidity_str.c_str();
            const char * state_const = state_str.c_str();

            printf("%s\n", humidity_const);
            printf("%s\n\n",state_const);

            LCD.print(humidity_const);
            LCD.setCursor(0, 2);
            LCD.print(state_const);

            //Buzzer control
            if(state == "Harmful" && muted == false){
                GPIOB->ODR &=~(0x8000);
            }
            else if(state == "Fair" || state == "Ideal" || (state == "Harmful" && muted == true)){
                GPIOB->ODR |= 0x8000;
                mux.lock();
                muted = false;
                mux.unlock();
            }
            Watchdog::get_instance().kick();
        }   
}

/*
Function Name: mute()
Function Purpose: This function is the ISR for the interrupt
                  "mute_pressed". If muted == 0, then this 
                  function will set it to 1. And vice versa.
Inputs: bool muted; 
Outputs: muted = 1;
Constraints:
*/
void mute(){
        mute_pressed.disable_irq();
        osDelay(500);
        if(state == "Harmful" && muted == false){
            muted = true;
        }
        osDelay(500);
        mute_pressed.enable_irq();
}


