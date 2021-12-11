
/* 
Author: Ronald Chen 
Course: CSE321
Creation Date: 12/5/2021
Program Name: Humidity Alarm
Program Summary: This program is the software required to operate the humidity alarm being designed. 
                 The humidity alarm is a device that detects the humidity levels in a room, and alarms 
                 the user when the humidity percentage becomes too high or low via the buzzer audio module.
                 The buzzer will continue to output noise until levels return to normal or the user button B1
                 on the Nucleo is pressed. Once the buzzer is muted, it cannot be unmuted and will remain as such 
                 until the  state returns to "Fair" or "Ideal" again. This design choice is modelled after 
                 smoke detectors.The humidity percentage and state will be printed onto the LCD and serial monitor. 
Inputs: DHT11 humidity data, User button B1
Outputs: Humidity percentage, and state printed to LCD, Buzzer audio output.
Implementation Requirements: Watchdog timer, Synchronization technique, Bitwise driver configuration,
                             Critical section protection, Use of threads/tasks, 1 Interrupt.
Required Materials: Nucleo-L4R5ZI, DHT11 Sensor, Buzzer audio module, 16x2 LCD Display, Wires, Resistors,
                    Breadboard, USBA to Micro USB B.
Additional Info: The buzzer is activated on an actived on an active low signal. 
                 Sampling period of DHT11 must be > 2 seconds.
                 Watchdog times out if not kicked within 2.2 seconds.
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


int sense_interval = 2100;              // DHT11 Senses every 2.1 seconds.
const uint32_t watchdog_timeout = 3000; // Watchdog times out after 2.2 seconds.        

Thread t_sense;                         // This thread will sense humidity.
Thread t_display;                       // This thread will update the LCD
Thread t_audio;                         // This thread will control the buzzer.
InterruptIn mute_pressed(PC_13);        // User Button is the mute button.
void sense();
void display();
void audio();
void mute();
void initialize_watchdog(int);

Queue<int,1> humidityQue;
Queue<string,1> display_stateQue;
DHT11 sensor(PC_6);
CSE321_LCD LCD(16,2,LCD_5x10DOTS,PB_9,PB_8);

bool muted;
string state;
int humidity;
int main()
{
    LCD.begin();
    initialize_watchdog(watchdog_timeout);

    RCC->AHB2ENR |= 0x6;            //Enable ports B and C.
    GPIOB->MODER &=~(0x800A0000);   //Configure GPIOB Moder: Output (01) for pins 8(SCL), 9(SDA) & 15(Buzzer).
    GPIOB->MODER |= 0x40050000;
    GPIOC->MODER &=~(0xC003000);    //Configure GPIOC Moder: Input (00) for pins 6(DHT) & 13(User Button).
    GPIOB->ODR |= 0x8000;           //Disable buzzer noise: PB_15
    GPIOC->IDR |= 0x40;             //Configure DHT11 IDR: PC_6

    t_sense.start(sense);  
    t_display.start(display);
    t_audio.start(audio);
    mute_pressed.rise(mute);
    
}

/*
Function Name: sense()
Function Purpose: This function is executed by the thread t_sense,
                  and will continuouslly read data from the DHT11 
                  sensor, and update variables "state", and "humidity"
                  accordingly. 
                  Saftey Levels: 
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
        sensor.read();
        int humidity_data = sensor.getHumidity();
        string state_data;
        if(humidity_data < 30 || humidity_data < 60 ){
            state_data = "Ideal";
        }
        else if ((humidity_data >= 25 && humidity_data <= 30) || (humidity_data >= 60 && humidity_data <= 70)) {
            state_data = "Fair";
        }
        else if(humidity_data < 30 || humidity_data > 70){
            state_data = "Harmful";
        }
        display_stateQue.try_put(&state_data);
        humidityQue.try_put(&humidity_data);
    }
}

/*
Function Name: display()
Function Purpose: This function will wait for humidity and state 
                  data from the two Queues and display the humidity 
                  and state on the LCD display, and serial monitor 
                  when received. If data isn't received from the 
                  queue within 3 seconds, the watchdog timer will
                  reset the program. 
Inputs: int humidity, string level.
Outputs: Update LCD everytime data is given to the queue changes.
         Activates buzzer if state == "Harmful", and disables it
         otherwise.
Constraints: Can only update as fast as sensor is able to sense.
 */
void display(){ 
        while(true){
            // Wait for data to be sent to both queues.
            osEvent humidityMsg = humidityQue.get(osWaitForever);
            osEvent stateMsg = display_stateQue.get(osWaitForever);
            
            //Extract the values and update global variables.
            state = *(string*)stateMsg.value.p;
            humidity = *(int*)humidityMsg.value.p;

            //Construct the strings to display
            string humidity_str = "Humidity: "+ std::to_string(*(int*)humidityMsg.value.p) + "%";
            string state_str = "State: " + *(string*)stateMsg.value.p;
            
            //Convert std::strings to c strings so it can be printed.
            const char * humidity_const = humidity_str.c_str();     
            const char * state_const = state_str.c_str();

            //Refresh the LCD and print to the serial monitor and LCD.
            LCD.clear();
            printf("%s\n", humidity_const);
            printf("%s\n\n",state_const);
            LCD.print(humidity_const);
            LCD.setCursor(0, 2);
            LCD.print(state_const);

            //Data has been processed, and updated sucessfully, reset the watchdog timer.
            Watchdog::get_instance().kick();
        }   
}

/*
Function Name: audio()
Function Purpose: This function continuously monitors the variables "state" and "muted". 
                  It will activate the buzzer if the state is "Harmful" and isn't muted, 
                  deactivate it if it is "Fair" or "Ideal", or muted = true. Once muted, 
                  the buzzer will not output until levels return to fair or normal,
                  similar to a smoke detector.
Inputs: bool muted, string state. 
Outputs: Buzzer activation/deactivaton.
Constraints:
*/
void audio(){
    while(true){
            if(state == "Harmful" && muted == false){
                GPIOB->ODR &=~(0x8000);
            }
            else if((state == "Harmful" && muted == true)){
                GPIOB->ODR |= 0x8000;
            }
            else if(state == "Fair" || state == "Ideal" ){
                GPIOB->ODR |= 0x8000;
                muted = false;
            }
    }
}
/*
Function Name: mute()
Function Purpose: This function is the ISR called on the rising edge 
                  of the interrupt "mute_pressed". It will turn off
                  the buzzer if state is "Harmful", and isn't currently
                  muted. 
Inputs: bool muted; 
Outputs: muted = 1;
Constraints: Bounce must be accounted for, as a result there is
             a delay incorporated, and interrupt requests are
             disabled.
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



/*
Function Name: initialize_watchdog()
Function Purpose: This function is called at the beginning of the program
                  and initializes the watchdog timer. It will print out
                  if the timer has been initialized sucessfully or not.
Inputs: int timeout: Time until watchdog times out
Outputs: Prints if timer has  initialized sucessfully or not.
Constraints: 
*/
void initialize_watchdog(int timeout){
    Watchdog &watchdog = Watchdog::get_instance();  
    if(watchdog.start(timeout) == true && watchdog.is_running() == true){
        printf("Watchdog timer started sucessfully \nTimeout initialized to: %ims\n\n", watchdog.get_timeout());
    }
    else{
        printf("Watchdog timer failed to start.\n");
    }
}