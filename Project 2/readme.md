-------------------
About
-------------------
Author: Ronald Chen

Project Description: The goal of this project is create a user-configurable timer through the combinaion of software and hardware. The user will be able to input a time up to a total of 9 minutes and 59 seconds. The user may interact with the system through a 4x4 keypad connected to a NUCLEO-L4R5ZI. The remaining time is displayed on a 16x2 LCD display, that updates the time remmaining in real time. The user may pause the timer in this state as well, or input a new time. Once the countdown is complete the LCD will display "Times Up!" followed by the flashing of all four external LEDs.

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
- 1802 LCD display
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
Connect the Nucleo to a computer with a USB A to Micro USB B cable. Setup a power and ground connection to the breadboard. Connect the external peripherals to the correct pins on the Nucleo-L4R5ZI with resistors and wires.

Keypad:
 Column 1: PC_6
 Column 2: PB_15
 Column 3: PB_13
 Column 4: PB_12
 Row 5: PC_12   
 Row 6: PD_2
 Row 7: PF_3
 Row 8: PF_5
 
LCD:
 VCC: 3.3V
 GND: GND
 SDA: PB_9
 SCL: PB_8

LEDs:
 Blue: PA_3
 Green: PC_0
 Red:  PC_3
 White: PC_1

More detailed information on pins and connections are detailed in the pdf documentation. 

--------------------
CSE321_project2_rchen56_main.cpp
--------------------
 This file contains the code that drives the timer. 

----------
Declarations
----------
void isr_0(void);
void isr_1(void);
void isr_2(void);
void isr_3(void);
void isr_4(void);
void isr_5(void);
void isr_6(void);
void isr_7(void);
void isr_8(void);
void isr_9(void);
void isr_a(void);
void isr_b(void);
void isr_d(void);
void col1_press(void);
void col2_press(void);
void col3_press(void);
void col4_press(void);

InterruptIn col1(PC_6,PullDown);
InterruptIn col2(PB_15,PullDown);
InterruptIn col3(PB_13,PullDown);
InterruptIn col4(PB_12,PullDown);

int row = 5;
bool inputting =true;
bool counting = false;
bool paused = false;
bool times_up = false;

bool inputting_minutes;
bool inputting_sec10;
bool inputting_sec1;

int minutes;
int sec10;
int sec1;

char zero = '0';
char one = '1';
char two = '2';
char three = '3';
char four = '4';
char five = '5';
char six = '6';
char seven = '7';
char eight = '8';
char nine = '9';
char colon = ':';
char underscore = '_';

char *zeroPtr = &zero;
char *onePtr =  &one;
char *twoPtr = &two;
char *threePtr = &three;
char *fourPtr = &four;
char *fivePtr = &five;
char *sixPtr = &six;
char *sevenPtr = &seven;
char *eightPtr = &eight;
char *ninePtr = &nine;
char *colonPtr = &colon;
char *minutes_str = &underscore;
char *sec10_str = &underscore;
char *sec1_str = &underscore;

void count(void);
void pause(void);
void input_minutes();
void input_sec10();
void input_sec1();

CSE321_LCD LCD(16,2,LCD_5x8DOTS,PB_9,PB_8);

----------
API and Built In Elements Used
----------
- 1802.cpp
- 1802.h
- mbed.h

