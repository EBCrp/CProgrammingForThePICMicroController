/*
 Traffic Light Example
 Copied by E. Cadel dated 1/12/2024 for the 
 18f4525 PIC
 C Programming for the PIC Microcontroller, Chapter 4
 No modifications to date 
 */


// PIC18F4525 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config OSC = INTIO67    // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/*
 * Definitions - it is useful to allocate symbolic names to the actual bits on the I/O.
 * The symbolic names should give some suggestion as to what the I/O is used for.
 * Now wherever the compiler sees the symbolic name, it knows what the I/O really means.
 * Doing this also makes it easier to change the allocation list if needed.
 * Note that the line with the definition does not end with the semicolon.
 * This is because it is not an instruction for the program, it is just a statement
 * for the compiler software to be aware of.
 */

#include <xc.h>
#define _XTAL_FREQ (8000000)

#define redLamp1 PORTBbits.RB0      
#define amberLamp1 PORTBbits.RB1    
#define greenLamp1 PORTBbits.RB2    

void main(void) {
    
PORTA = 0;
PORTB = 0;
PORTC = 0;
PORTD = 0;
PORTE = 0;
TRISB = 0;      // Sets all bits on PORTB to outputs
ADCON0 = 0;     // Turn off ADC
ADCON1 = 0x0F;  // Make all bits digital
OSCCON = 0x74;  // Set OSC to 8Mhz with stable output
T0CON = 0xC7;   // Set TMR0 to 8 bit register with divide by 256 rate so runs at 7812.5Hz, one tick = 128us

while (1)       // Start of forever loop so micro carries out start of loop only once
{
    redLamp1 = 1;
    __delay_ms (5000);    // Call subroutine 'delay' and passes value 153 up to the subroutine to create 5 second delay
    amberLamp1 = 1;
    __delay_ms (2000);     // 2 second delay
    redLamp1 = 0;
    amberLamp1 = 0;
    greenLamp1 = 1;
    __delay_ms (5000);
    greenLamp1 = 0;
    amberLamp1 = 1;
    __delay_ms (2000);
    amberLamp1 = 0;
    
}
}
