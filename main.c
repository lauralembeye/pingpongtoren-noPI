/* Demo project pingpongtoren + hoogtesensor 
 * 
 * 
 * pinout:  RC2 = receiver input
 *          RC7 = transmitter output
 *          RB6 = pulse lengte output
 *          RB4 = pwm output
 *          RC1 = motor output
 */

#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "PI.h"
#include "UART.h"
#include "blok2binair.h"

/*
                         Main application
 */
static uint8_t blue = 0x1F, green = 0x1F, red = 0x1F;

enum states {
    GREEN_UP, RED_DOWN, BLUE_UP, GREEN_DOWN, RED_UP, BLUE_DOWN
};
enum states change_color = GREEN_UP;
uint8_t step = 1;

enum count {
    UP, DOWN
};
enum count direction = UP;
char led_run = 0;
static uint8_t hoogte_bal = 0;
#define NumberOfLEDs  58
int teller = 0;
char flag = 0;

void seed(void){
           
    srand(TMR3_ReadTimer());
    led_run = rand()%NumberOfLEDs;
    flag = 1;
            
}
uint8_t GetHoogte_bal(void){
        return hoogte_bal;
    }
void Send_LED_Frame(uint8_t intensity, uint8_t blue, uint8_t green, uint8_t red) {
    SPI1_ExchangeByte(0xE0 | intensity);
    SPI1_ExchangeByte(blue);
    SPI1_ExchangeByte(green);
    SPI1_ExchangeByte(red);
}

void Send_LED_StartFrame() {
    for (int i = 0; i < 4; i++) {//3kleuren
        SPI1_ExchangeByte(0x00);
    }
}

void Send_LED_EndFrame() {
    for (int i = 0; i < 4; i++) {
        SPI1_ExchangeByte(0xFF);
    }
}

void check_balhoogte(void){
           if (hoogte_bal-5<= led_run && led_run <=hoogte_bal+5){
               teller=teller+1;
               led_run = rand()%NumberOfLEDs; 
               printf("%u", led_run);
               //printf("%u", led_run);
           }
           /*if (hoogte_bal==led_run){
               teller=teller+1;
               led_run = rand()%NumberOfLEDs;
           }*/
           
        }  



void main(void) {
    // initialize the device
    SYSTEM_Initialize();
    if(!spi_master_open(MASTER0)){
        // Not able to open SPI, call fail() or optionally do something 
        // else waiting for the SPI to become free
        return ; 
    }


    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    // Initialiseer de hoogtemeting
    ADC_SelectChannel(Hoogtesensor);
    ADC_StartConversion();
    TMR2_Initialize();
    TMR2_StartTimer();
    TMR3_StartTimer();
    INT_SetInterruptHandler(seed);

    
    
    //printf("Hello :) \r\n");
    hoogte_bal = PI_GetSensorHeight()/3.36;
    printf("%u" ,GetHoogte_bal());
    while (1) {
        loop();
        uartHandler();
        
 //        PI moet op een vaste frequentie (elke 33ms) lopen voor de integrator
        if (TMR0_HasOverflowOccured()) {
            TMR0_Initialize();

            PI();
            static uint8_t printCycle = 0; //door static toe te voegen wordt "printCycle" niet elke keer her geinitialiseerd maar behoudt het zijn vorige waarde
            if (printCycle++ > 30) {
                printLogs();
                printCycle = 0;
            }
        }
        if(get_getal()== 0x807F8877){
            int x = (int)PI_GetSetPoint() + 5;
            if(x>=255){
                x = 255;
            }
            PI_SetSetpoint((uint8_t)x);
            set_getal();
            //printf("%u \n",PI_GetSetPoint());
        }
        if(get_getal()== 0x807F9867){
            int x = PI_GetSetPoint() - 5;
            if(x<=0){
                x = 0;
            }
            PI_SetSetpoint((uint8_t)x);
            set_getal();
        }
        //kleurverandering
        switch (change_color) {
            case GREEN_UP: if (green < 0xFF) {
                    green += step;
                } else {
                    change_color = RED_DOWN;
                }
                break;
            case RED_DOWN: if (red > 0x00) {
                    red -= step;
                } else {
                    change_color = BLUE_UP;
                }
                break;
            case BLUE_UP: if (blue < 0xFF) {
                    blue += step;
                } else {
                    change_color = GREEN_DOWN;
                }
                break;
            case GREEN_DOWN: if (green > 0x00) {
                    green -= step;
                } else {
                    change_color = RED_UP;
                }
                break;
            case RED_UP: if (red < 0xFF) {
                    red += step;
                } else {
                    change_color = BLUE_DOWN;
                }
                break;
            case BLUE_DOWN: if (blue > 0x00) {
                    blue -= step;
                } else {
                    change_color = GREEN_UP;
                }
                break;
        }
            
        check_balhoogte();
        
        //zet licht aan en uit
        //start frame
        Send_LED_StartFrame();
        if(flag == 1){
            for (char led = 0; led < NumberOfLEDs; led++) {
                loop();
                if (led == led_run) {
                    Send_LED_Frame(0x1F, blue,  green, red);
                } 
                else {
                    Send_LED_Frame(0x00, 0x00, 0x00, 0x00);
                }
            }
        }
        else{
            for (char led = 0; led < NumberOfLEDs; led++) {
                                loop();

            Send_LED_Frame(0x00, 0x00, 0x00, 0x00);
            }
        }

   

    }
    //stop frame
    Send_LED_EndFrame();
    __delay_ms(1000);
    

}

/**
 End of File
 */