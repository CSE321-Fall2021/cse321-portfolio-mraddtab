-------------------
About
-------------------
Author: Ronald Chen

Project Description: The goal of this project is to create a "Humidity Alarm" system through the combination of hardware and software. This device will monitor the humidity level of a home with the DHT11 sensor, which is capable of detecting humidty and temperature. The humidity percantage and saftey level (Ideal, Fair, Harmful) will be displayed on the 16x2 LCD display. When humidity drops below 25% or rise above 75%, the buzzer device will output audio signals continuously until humidity levels are safe, or is manually muted by the user with the on-board user button. The motivation behind this project is to create a device that prevents the harmful effects which may occur if low/high humidity levels are left undetected.

--------------------
Features
--------------------
- Runs Forever
- Realtime display.
- Detects humidity levels.
- Displays current humidity percentage.
- Audio warning when harmful humidity levels are detected.
- User interactive.

--------------------
Required Materials
--------------------
- Nucleo L4R5ZI
- Solderless Breadboard
- Jumper wires
- 1802 LCD display
- USB A to Micro USB B cable
- Resistors 
- DHT11 Sensor
- Buzzer Alarm Module

--------------------
Resources and References
--------------------
- UM2179 User Manual: https://www.st.com/resource/en/user_manual/dm00368330-stm32-nucleo-144-boards-mb1312-stmicroelectronics.pdf. 
- RM0432 Reference Manual: https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
- LCD Manual: https://learn-us-east-1-prod-fleet02-xythos.content.blackboardcdn.com/5e00ea752296c/10911180?X-Blackboard-Expiration=1634364000000&X-Blackboard-Signature=MOnja6ZGfmHtNs46fE12cbxFgsWLDdX4ZfLcV9TC9Es%3D&X-Blackboard-Client-Id=100310&response-cache-control=private%2C%20max-age%3D21600&response-content-disposition=inline%3B%20filename%2A%3DUTF-8%27%27JDH_1804_Datasheet.pdf&response-content-type=application%2Fpdf&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20211016T000000Z&X-Amz-SignedHeaders=host&X-Amz-Expires=21600&X-Amz-Credential=AKIAZH6WM4PL5SJBSTP6%2F20211016%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Signature=6fa8d9a1bdf470eda9395760ffff2ab2ed7a58c10a191064bf3342ddedc1d5ee
- Buzzer Reference: http://tinkbox.ph/sites/tinkbox.ph/file/downloads/5V_BUZZER_MODULE.pdf
- DHT11 Reference: https://components101.com/sites/default/files/component_datasheet/DHT	11-Temperature-Sensor.pdf 

--------------------
Getting Started
--------------------
Connect the Nucleo to a computer with a USB A to Micro USB B cable. Setup a power and ground connection to the breadboard. Connect the external peripherals to the correct pins on the Nucleo-L4R5ZI with resistors and wires. Currently the hardware schematic has not been completed.

--------------------
Additional Files
--------------------
- 1802.cpp
- 1802.h
- DHT11.cpp
- DHT11.h
----------
Declarations
----------
- int sense_interval = 2100;              
- const uint32_t watchdog_timeout = 2200;       
- Thread t_sense;                         
- Thread t_display;                       
- Thread t_audio;                         
- InterruptIn mute_pressed(PC_13);        
- void sense();
- void display();
- void audio();
- void mute();
- void initialize_watchdog(int);
- Queue<int,1> humidityQue;
- Queue<string,1> display_stateQue;
- DHT11 sensor(PC_6);
- CSE321_LCD LCD(16,2,LCD_5x10DOTS,PB_9,PB_8);
- bool muted;
- string state;
- int humidity;
