
/*
* File: CSE321_project1_rchen56_main.cpp
*
* Author: Ronald Chen (rchen56@buffalo.edu)
* Date: Oct 14, 2021
* Course: CSE321
* Required Materials: Nucleo L4R5ZIucleo L4R5ZI, Solderless Breadboard, Jumper wires, 1802 LCD display, 
*                     USB A to Micro USB B cable, 4 LEDs, Resistors, 4x4 Keypad
*                    
* Inputs: Keypad buttons
*
* Outputs: Time displayed on LCD, blinking external LEDs.
*
*
* Summary of File: 
*               This file contains the code that will drive the keypad and LCD display. There will be 4 states
*               Inputting Time, Timer counting, Timer paused, and Count Complete. Four external LEDs will describe the program 
*               state: White LED: Inputting Time. 
*                | LED   |     Behavior     |       LCD Prompt        |                      
*                -----------------------------------------------------|
*                | Blue  | Digit is entered.| Input Time: m:ss        |
*                | White | Inputting Time   | Input Time: m:ss        |
*                |  Red  | Timer Paused     | "Time Remaining: m:ss"  |
*                | Green | Time Counting.   | "Time Remaining: m:ss"  |
*                |  All  | Done Counting.   | "Times Up"              |
*               
*                
*               
*/

#include "mbed.h"
#include  "1802.h"
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <iterator>
#include <string>

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

int main(){

    LCD.begin();
    LCD.clear();
    //Enable RCC, Ports A,B,C,D,F
    RCC->AHB2ENR |= 0x2F;
    RCC->AHB2ENR &=~(0xD0);

    //Enable Moder output (01) to
    //Row 5 -> PC_12, Row 6 -> PD_2
    //Row 7 -> PF_3,  Row 8 -> PF5.         
    GPIOC->MODER |= 0x1000000; //PC12
    GPIOC->MODER &=~(0x2000000);
    GPIOD ->MODER |= 0x10; //PD2
    GPIOD ->MODER &=~(0x20);
    GPIOF->MODER |= 0x440; //PF3 & 5
    GPIOF->MODER &=~(0x880);
   
   /* Turn on output (01) for the LEDs:
    Blue LED: PA_3
    Green LED: PC_0
    RED LED: PC_3
    White LED: PC_1 */
    
    GPIOA->MODER |= 0x40;
    GPIOA->MODER &=~ 0x80;
    GPIOC->MODER |= 0x45;
    GPIOC->MODER &=~(0x8A);
    //Detects electric charge from 
    //closing switch from button press.
    col1.rise(&col1_press);
    col2.rise(&col2_press);
    col3.rise(&col3_press);
    col4.rise(&col4_press);
        
    //Initialize to "inputting" state. 
    LCD.print("Enter Time");
    inputting_minutes = true;
    inputting = true;
    GPIOC->ODR |=0x2; 
    //Cycle power to rows.
    while(1){
        if(row > 8) row =5;
        if(row == 5){                       //Turn on PC_12, turn off PD_2, PF_3 and PF_5
            GPIOC->ODR |= 0x1000;        
            GPIOD->ODR &=~(0x4);        
            GPIOF->ODR &=~(0x8);
            GPIOF->ODR &=~(0x20);
        }  
        else if(row == 6){                       //Turn on PD_2, turn off PC_12, PF_3 and PF_5
            GPIOC->ODR &=~(0x1000);        
            GPIOD->ODR |=(0x4);        
            GPIOF->ODR &=~(0x8);
            GPIOF->ODR &=~(0x20);
        }
        else if(row == 7){                       //Turn on PF_3, turn off PC_12, PD_2 and PF_5
            GPIOC->ODR &=~(0x1000);       
            GPIOD->ODR &=~(0x4);        
            GPIOF->ODR |=(0x8);
            GPIOF->ODR &=~(0x20);
        }
        else if(row == 8 ){                      //Turn on PF_5, turn off PC_12, PD_2 and PF_3
            GPIOC->ODR &=~(0x1000);      
            GPIOD->ODR &=~(0x4);        
            GPIOF->ODR &=~(0x8);
            GPIOF->ODR |=(0x20);
        }
        wait_us(50000);
        row+=1;

        if(inputting_minutes){
            input_minutes();
        }
        else if(inputting_sec10){
            input_sec10();
        }   
        else if(inputting_sec1){
            input_sec1();
        }
        if(counting){
            count();
        }
        if(paused){
            pause();
        }
        if(times_up){
            LCD.clear();
            LCD.setCursor(5,0);
            LCD.print("Times");
            LCD.setCursor(7, 1);
            LCD.print("Up");
            GPIOA->ODR |= 0x8;
            GPIOC->ODR |= 0xB;
        }
    }
return 0;
}

  
/*************
Function Name: col1_press()

Summary: ISR For keypad inputs in column 1.
        Row 5 | D Pressed, Input time.   | White Led | PC_1|  GPIOC-> ODR |= 0x2;
        Row 7 | B Pressed, Stop/Turn Off.| Red LED   |PC_3     GPIOC->ODR |= 0x8;     
        Row 8 | A Pressed, Start Timer.  | Green Led | PC_0    GPIOC->ODR |= 0x1;   
Parameters: Void

Return: Void
**************/
void col1_press(void){
    if(row == 5)  isr_d();
    else if(row==7) isr_b();
    else if(row == 8) isr_a();    
}

/*************
Function Name: col2_press()

Summary: ISR For keypad inputs in column 2
         Blink blue LED when digit entered
               | Row 6 | 9 Pressed | 
               | Row 7 | 6 Pressed |     
               | Row 8 | 3 Pressed | 
Parameters: Void

Return: Void
**************/
void col2_press(void){
    if(inputting){
        if(row == 6) isr_9();
        else if(row ==7) isr_6();
        else if(row==8)isr_3();
    }
}

/*************
Function Name: col3_press()

Summary: ISR For keypad inputs in column 2
         Blink blue LED when digit entered
               | Row 5 | 0 Pressed | 
               | Row 6 | 8 Pressed | 
               | Row 7 | 5 Pressed |     
               | Row 8 | 2 Pressed | 
Parameters: Void

Return: Void
**************/
void col3_press(void){
    if(inputting){
        if(row == 5) isr_0();
        if(row == 6) isr_8();
        else if(row ==7) isr_5();
        else if(row==8) isr_2();
    }
}

/*************
Function Name: 

Parameters 

Return: None
**************/
void col4_press(void){
   if(inputting){
    if(row == 6){
        isr_7();
        }
    else if(row ==7){
        isr_4();
        }
    else if(row==8){
        isr_1();
        }
    }
}


void isr_0(void){
    GPIOA->ODR |=0x8;
    wait_us(5000);
    GPIOA->ODR &=~(0x8);
    if(inputting_minutes){
        minutes_str = zeroPtr;
        inputting_minutes = false;
        inputting_sec10 = true;
        inputting_sec1 = false;
        return;
    }
    else if(inputting_sec10){
        sec10_str = zeroPtr;
        inputting_minutes = false;
        inputting_sec10 = false;
        inputting_sec1 = true;
        return;
    }
    else if(inputting_sec1){
        sec1_str = zeroPtr;
        inputting_minutes = false;
        inputting_sec10 = false;
        return;
    }
    GPIOA->ODR |=0x8;
    wait_us(5000);
    GPIOA->ODR &=~(0x8);    
}

void isr_1(void){
    GPIOA->ODR |=0x8;
    wait_us(5000);
    GPIOA->ODR &=~(0x8);
    if(inputting_minutes){
        minutes_str = onePtr;
        inputting_minutes = false;
        inputting_sec10 = true;
        inputting_sec1 = false;
        return;
    }
    else if(inputting_sec10){
        sec10_str = onePtr;
        inputting_minutes = false;
        inputting_sec10 = false;
        inputting_sec1 = true;
        return;
    }
    else if(inputting_sec1){
        sec1_str = onePtr;
        inputting_minutes = false;
        inputting_sec10 = false;
        return;
    }
    GPIOA->ODR |=0x8;
    wait_us(5000);
    GPIOA->ODR &=~(0x8);    
}

void isr_2(void){
    GPIOA->ODR |=0x8;
    wait_us(5000);
    GPIOA->ODR &=~(0x8);
    if(inputting_minutes){
        minutes_str = twoPtr;
        inputting_minutes = false;
        inputting_sec10 = true;
        inputting_sec1 = false;
        return;
    }
    else if(inputting_sec10){
        sec10_str = twoPtr;
        inputting_minutes = false;
        inputting_sec10 = false;
        inputting_sec1 = true;
        return;
    }
    else if(inputting_sec1){
        sec1_str = twoPtr;
        inputting_minutes = false;
        inputting_sec10 = false;
        return;
    }
    GPIOA->ODR |=0x8;
    wait_us(5000);
    GPIOA->ODR &=~(0x8);  
}

void isr_3(void){
    GPIOA->ODR |=0x8;
    wait_us(5000);
    GPIOA->ODR &=~(0x8);
    if(inputting_minutes){
        minutes_str = threePtr;
        inputting_minutes = false;
        inputting_sec10 = true;
        inputting_sec1 = false;
        return;
    }
    else if(inputting_sec10){
        sec10_str = threePtr;
        inputting_minutes = false;
        inputting_sec10 = false;
        inputting_sec1 = true;
        return;
    }
    else if(inputting_sec1){
        sec1_str = threePtr;;
        inputting_minutes = false;
        inputting_sec10 = false;
        return;
    }
    GPIOA->ODR |=0x8;
    wait_us(5000);
    GPIOA->ODR &=~(0x8);    
}

void isr_4(void){
    GPIOA->ODR |=0x8;
    wait_us(5000);
    GPIOA->ODR &=~(0x8);
    if(inputting_minutes){
        minutes_str = fourPtr;
        inputting_minutes = false;
        inputting_sec10 = true;
        inputting_sec1 = false;
        return;
    }
    else if(inputting_sec10){
        sec10_str = fourPtr;
        inputting_minutes = false;
        inputting_sec10 = false;
        inputting_sec1 = true;
        return;
    }
    else if(inputting_sec1){
        sec1_str = fourPtr;;
        inputting_minutes = false;
        inputting_sec10 = false;
        return;
    }
};

void isr_5(void){
    GPIOA->ODR |=0x8;
    wait_us(5000);
    GPIOA->ODR &=~(0x8); 
    if(inputting_minutes){
        minutes_str = fivePtr;
        inputting_minutes = false;
        inputting_sec10 = true;
        inputting_sec1 = false;
        return;
    }
    else if(inputting_sec10){
        sec10_str = fivePtr;
        inputting_minutes = false;
        inputting_sec10 = false;
        inputting_sec1 = true;
        return;
    }
    else if(inputting_sec1){
        sec1_str = fivePtr;;
        inputting_minutes = false;
        inputting_sec10 = false;
        return;
    }
}

void isr_6(void){
    if(inputting_minutes){
        minutes_str = sixPtr;
        inputting_minutes = false;
        inputting_sec10 = true;
        inputting_sec1 = false;
        GPIOA->ODR |=0x8;
        wait_us(5000);
        GPIOA->ODR &=~(0x8);
        return;
    }
    else if(inputting_sec1){
        sec1_str = sixPtr;
        inputting_minutes = false;
        inputting_sec10 = false;
        GPIOA->ODR |=0x8;
        wait_us(5000);
        GPIOA->ODR &=~(0x8);
        return;
    }    
}

void isr_7(void){
    if(inputting_minutes){
        minutes_str = sevenPtr;
        inputting_minutes = false;
        inputting_sec10 = true;
        inputting_sec1 = false;
        GPIOA->ODR |=0x8;
        wait_us(5000);
        GPIOA->ODR &=~(0x8);
        return;
    }
    else if(inputting_sec1){
        sec1_str = sevenPtr;
        inputting_minutes = false;
        inputting_sec10 = false;
        GPIOA->ODR |=0x8;
        wait_us(5000);
        GPIOA->ODR &=~(0x8);
        return;
    }    
}

void isr_8(void){
    if(inputting_minutes){
        minutes_str = eightPtr;
        inputting_minutes = false;
        inputting_sec10 = true;
        inputting_sec1 = false;
        GPIOA->ODR |=0x8;
        wait_us(5000);
        GPIOA->ODR &=~(0x8);
        return;
    }
    else if(inputting_sec1){
        sec1_str = eightPtr;
        inputting_minutes = false;
        inputting_sec10 = false;
        GPIOA->ODR |=0x8;
        wait_us(5000);
        GPIOA->ODR &=~(0x8);
        return;
    }    
}

void isr_9(void){
    if(inputting_minutes){
        minutes_str = ninePtr;
        inputting_minutes = false;
        inputting_sec10 = true;
        inputting_sec1 = false;
        GPIOA->ODR |=0x8;
        wait_us(5000);
        GPIOA->ODR &=~(0x8);
        return;
        }
    else if(inputting_sec1){
        sec1_str = ninePtr;
        inputting_minutes = false;
        inputting_sec10 = false;
        GPIOA->ODR |=0x8;
        wait_us(5000);
        GPIOA->ODR &=~(0x8);
        return;
    }  
}


void count(){
        LCD.clear();
        LCD.setCursor(1,0);
        LCD.print("Time remaining:");
        LCD.setCursor(7, 1);
        LCD.print(minutes_str);
        LCD.print(colonPtr);
        LCD.print(sec10_str);
        LCD.print(sec1_str);
        GPIOF->ODR |= 0x8;
        wait_us(1000000);
        GPIOF->ODR &=~ (0x8);
        if(sec1_str == zeroPtr){ 
            if(sec10_str == zeroPtr){
                if(minutes_str == zeroPtr){
                    counting = false;
                    times_up = true;   
                }
                else{
                    sec10_str = fivePtr;
                    sec1_str = ninePtr;
                    if(minutes_str == ninePtr){
                        minutes_str = eightPtr;
                    }
                    else if(minutes_str == eightPtr){
                        minutes_str = sevenPtr;
                    }
                    else if(minutes_str == sevenPtr){
                        minutes_str = sixPtr;
                    }
                    else if(minutes_str == sixPtr){
                        minutes_str = fivePtr;
                    }
                    else if(minutes_str == fivePtr){
                        minutes_str = fourPtr;
                    }
                    else if(minutes_str == fourPtr){
                        minutes_str = threePtr;
                    }
                    else if(minutes_str == threePtr){
                        minutes_str = twoPtr;
                    }
                    else if(minutes_str == twoPtr){
                        minutes_str = onePtr;
                    }
                    else if(minutes_str == onePtr){
                        minutes_str = zeroPtr;
                    }
                }
            }
            else{
                sec1_str = ninePtr;
                if(sec10_str == fivePtr){
                    sec10_str = fourPtr;
                }
                else if(sec10_str == fourPtr){
                    sec10_str = threePtr;
                }
                else if(sec10_str == threePtr){
                    sec10_str = twoPtr;
                }
                else if(sec10_str == twoPtr){
                    sec10_str = onePtr;
                }
                else if(sec10_str == onePtr){
                    sec10_str = zeroPtr;
                }
            }
        }
        else{
            if(sec1_str == ninePtr){
                sec1_str = eightPtr;
            }
            else if(sec1_str == eightPtr){
                sec1_str = sevenPtr;
            }
            else if(sec1_str == sevenPtr){
                sec1_str = sixPtr;
            }
            else if(sec1_str == sixPtr){
                sec1_str = fivePtr;
            }
            else if(sec1_str == fivePtr){
                sec1_str = fourPtr;
            }
            else if(sec1_str == fourPtr){
                sec1_str = threePtr;
            }
            else if(sec1_str == threePtr){
                sec1_str = twoPtr;
            }
            else if(sec1_str == twoPtr){
                sec1_str = onePtr;
            }
            else if(sec1_str == onePtr){
                sec1_str = zeroPtr;
            }
        }
}

void input_minutes(){
    LCD.clear();
    LCD.setCursor(3, 0);
    LCD.print("Enter Time: ");
    LCD.setCursor(7, 1);
    LCD.print(minutes_str);
    LCD.print(colonPtr);
    LCD.print(sec10_str);
    LCD.print(sec1_str);
}

void input_sec10(){
    LCD.clear();
    LCD.setCursor(3, 0);
    LCD.print("Enter Time: ");
    LCD.setCursor(7, 1);
    LCD.print(minutes_str);
    LCD.print(colonPtr);
    LCD.print(sec10_str);
    LCD.print(sec1_str);
}

void input_sec1(){
    LCD.clear();
    LCD.setCursor(3, 0);
    LCD.print("Enter Time: ");
    LCD.setCursor(7, 1);
    LCD.print(minutes_str);
    LCD.print(colonPtr);
    LCD.print(sec10_str);
    LCD.print(sec1_str);
}

void isr_a(){
    if(!counting && !times_up && !(minutes_str == &underscore || sec10_str == &underscore || sec1_str == &underscore)){
        counting = true;
        GPIOC->ODR |= 0x1;
        if(inputting){
            GPIOC->ODR &=~(0x2);
            inputting = false;
        }
        else if(paused){
            GPIOC->ODR &=~(0x8);
            paused = false;
        }
    }
}
void isr_b(){
    if(counting){
        counting = false;
        paused = true;
        GPIOC->ODR |= 0x8;
        GPIOC->ODR&=~(0x1);
    }
}
void isr_d(){
    inputting = true;
    inputting_minutes = true;
    inputting_sec10 = false;
    inputting_sec1 = false;
    minutes_str = &underscore;
    sec10_str = &underscore;
    sec1_str = &underscore;
    GPIOC->ODR |=0x2;
    if(counting){
        counting = false;
        GPIOC->ODR &=~(0x1);
    }
    else if(paused){
        paused = false;
        GPIOC->ODR &=~(0x8);
    }
    else if(times_up){
        times_up = false;
        GPIOC->ODR &=~(0xB);
        GPIOA->ODR &=~(0x8);
            
    }
}

void pause(){
    LCD.clear();
    LCD.setCursor(1,0);
    LCD.print("Time Paused:");
    LCD.setCursor(7, 1);
    LCD.print(minutes_str);
    LCD.print(&colon);
    LCD.print(sec10_str);
    LCD.print(sec1_str);
}