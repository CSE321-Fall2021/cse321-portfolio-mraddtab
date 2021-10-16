#include "mbed.h"
#include  "1802.h"
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
void isr_A(void);
void isr_B(void);
void isr_D(void);
void button_press(void);

InterruptIn col1(PC_8);
InterruptIn col2(PC_9);
InterruptIn col3(PC_10);
InterruptIn col4(PC_11);

int main()
{
    //Enable Port B and C
    RCC->AHB2ENR |= 0x6;
    //Keypad: Set MODER PB8-11 to be an input
    GPIOC->MODER &=~(0x110000);
    //LCD: Set MODER PB 13 & PB 12 to be output
    GPIOB->MODER &=~(0x8000000);
    GPIOB->MODER |= 0x4000000;
    GPIOB->MODER &=~(0x2000000);
    GPIOB->MODER |= 0x1000000;

    //Initialize LCD Object, SDA = PB13,SCL = PB12
    CSE321_LCD LCD(16,2,PB_13,PB_12);
    LCD.begin();
    while(1){
        LCD.print("Enter Time");
        thread_sleep_for(500);
    }
}
