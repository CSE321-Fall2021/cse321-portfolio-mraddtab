-------------------
About
-------------------
Author: Ronald Chen

Project Description: This project will create a user configure timer utilizing an LCD, Nucleo, and 4x4 Keypad.  

--------------------
Features
--------------------
- User Configuration.
- Realtime display.
- Maximum time is 9 minutes and 59 seconds.
- LED activation when timer reaches 0.

--------------------
Required Materials
--------------------
- Nucleo L4R5ZI
- Solderless Breadboard
- Jumper wires
- 1802 LCD displayer
- USB A to Micro USB B cable
- 4 LEDs
- 4x4 Keypad
- Resistors 

--------------------
Resources and References
--------------------
- UM2179 User Manual: https://www.st.com/resource/en/user_manual/dm00368330-stm32-nucleo-144-boards-mb1312-stmicroelectronics.pdf. 
- RM0432 Reference Manual: https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
- LCD Manual: https://learn-us-east-1-prod-fleet02-xythos.content.blackboardcdn.com/5e00ea752296c/10911180?X-Blackboard-Expiration=1634364000000&X-Blackboard-Signature=MOnja6ZGfmHtNs46fE12cbxFgsWLDdX4ZfLcV9TC9Es%3D&X-Blackboard-Client-Id=100310&response-cache-control=private%2C%20max-age%3D21600&response-content-disposition=inline%3B%20filename%2A%3DUTF-8%27%27JDH_1804_Datasheet.pdf&response-content-type=application%2Fpdf&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20211016T000000Z&X-Amz-SignedHeaders=host&X-Amz-Expires=21600&X-Amz-Credential=AKIAZH6WM4PL5SJBSTP6%2F20211016%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Signature=6fa8d9a1bdf470eda9395760ffff2ab2ed7a58c10a191064bf3342ddedc1d5ee

--------------------
Getting Started
--------------------
Connect the Nucleo to a computer with a USB A to Micro USB B cable. Setup a power and ground connection to the breadboard. Connect the 4x4 keypad to the corresponding pins on the Nucleo. Next set up the LCD display by connecting GND, VCC,SDA and SCL to the correct positions. More detailed information on pins and connections are detailed in the pdf documentation. 

--------------------
CSE321_project2_rchen56_main.cpp
--------------------
 This file contains the code that drives the timer. 

----------
Declarations
----------
void ISR_Button(void): 

InterruptIn col1(PC_8);

InterruptIn col2(PC_9);

InterruptIn col3(PC_10);

InterruptIn col4(PC_11);

CSE321_LCD LCD(16,2,PB_13,PB_12);

----------
API and Built In Elements Used
----------
- 1802.cpp
- 1802.h
- mbed.h

