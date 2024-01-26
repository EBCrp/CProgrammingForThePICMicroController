/*
 * File:   LCD_Programming_main.c
 * Author: Ellery Cadel
 * Copied from Chapter 6
 * 8-bit Version
 * Created on January 16, 2024
*
*/

// PIC18F4525 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config OSC = INTIO67      // Internal oscillator block, port function on RA6 and RA7
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

#include <xc.h>

// Use some comments to try and split the program listing into different sections

// Create any definitions
#define _XTAL_FREQ 8000000          // Oscillator frequency for delay
#define firstbyte   0b00110011      // Define the binary for first byte
#define secondbyte  0b00110011      // Define the binary for second byte
#define lines2bits8 0b00111100      // Define the binary for lines2bits8
#define eightBitOp  0b00111000      // Define the binary for leightbitOp
#define twoLines    0b00101100      // Define the binary for twoLines
#define incPosition 0b00000110      // Define the binary for incPosition
#define cursorNoBlink   0b00001100  // Define the binary for cursorNoBlink
#define clearScreen 0b00000001      // Define the binary for clearScreen
#define returnHome  0b00000010      // Define the binary for returnHome
#define lineTwo     0b11000000      // Define the binary for lineTwo
#define doBlink     0b00001111      // Define the binary for doBlink
#define shiftLeft   0b00010000      // Define the binary for shiftLeft
#define shiftRight  0b00010100      // Define the binary for shiftRight
#define lcdPort     PORTB           // Define which PORT LCD is connected to
#define eBit        PORTAbits.RA0   // Define which bit the LCD ebit is connected to
#define RSpin       PORTAbits.RA1   // Define which bit the LCD RSpin is connected to


// Some variables. These comments are just to split the listing into logical sections

unsigned char lcdInfo, lcdTempInfo, rsLine;   // Reserves 3 8-bit memory locations, 1 for each variable
unsigned char n;                            // Reserves an 8-bit memory location for variable 'n'
unsigned char str[80];       // Sets up 80 memory locations one after the other in an array. This array is used for sending a string to the LCD
char lcdInitialize [7] =    // Set up an array with 7 memory locations and load with the following LCD instructions
{                           // Opening bracket for array setup
    firstbyte,              // The first instruction
    secondbyte,             // The second instruction which is a copy of the first
    lines2bits8,              // The instruction to use 2 lines of characters and set up for 8-bit operation not 4-bit
    incPosition,            // Instruction to make the cursor move one place to the right after each data has been displayed on LCD
    doBlink,          // Instruction to make cursor blink in current position
    clearScreen,            // Instruction to clear all data from LCD display
    returnHome,             // Instruction to send back to first position on line 1 of LCD
};                          // End of array bracket. Note: the semi-colon is needed as the whole sequence is a program instruction

// The subroutines

void lcdOut ()              // Start of lcdOut subroutine
{
    lcdPort = lcdInfo;      // Send info to LCD
    eBit = 1;               // Set eBit to logic '1'
    eBit = 0;               // Set eBit to logic ''0'. These two make the LCD aware that new information has come to its input pins
    __delay_ms(2);
}

void setUpTheLCD ()         
{
    __delay_ms(33);
    RSpin = 0;              // Set RSpin to logic 0 to tell LCD information coming is an instruction
    n=0;                    // Set the variable n to 0 to ready next loop
    while (n < 7)
    {
        lcdInfo = lcdInitialize [n];     // Load variable lcdInfo with first instruction in array lcdInitialize
        lcdOut ();                     // Call subroutine lcdOut to send instruction to LCD
        n ++;
    }
    RSpin = 1;              // Set RSpin back to logic 1 as next information to go to LCD will most likely be data to be displayed
}

void line2 ()               // Start of subroutine to send cursor to start of line 2 on LCD
{
   RSpin = 0;               // Set RSpin to logic 0 to tell LCD information coming is an instruction
   lcdInfo = lineTwo;       // Load variable lcdInfo with instruction to go to lineTwo
   lcdOut ();               // Call lcdOut subroutine to send instruction to LCD
   RSpin = 1;               // Set RSpin back to logic 1 as next information to go to LCD will most likely be data to be displayed
}

void writeString (const char *words) // Subroutine that will display a whole string of characters on the display
{
    while (*words)          // While the *words pointer is not pointing to the NULL character do what is inside the brackets
    {
        lcdInfo = *words;   // Load what the *words pointer is pointing to into the variable lcdInfo
        lcdOut ();          // Call the subroutine to pass the Info to the LCD
        *words ++;          // increment the contents of the pointer so that it is pointing to the next char in the array
    }
}

void clearTheScreen ()      // Subroutine to clear all the data from the display and send cursor back to the start of the screen
{
    RSpin = 0;
    lcdInfo = clearScreen;   // Copy the data for clearScreen instruction into lcdInfo
    lcdOut ();              // Call the lcdOut subroutine to send instruction to LCD
    lcdInfo = returnHome;   // Copy the data for returnHome instruction into lcdInfo
    lcdOut ();
    RSpin = 1;
}

void main ()
{
    PORTA = 0;              // Turn all outputs off
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    TRISA = 0x00;           // Ports to output
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    ADCON0 = 0x00;           // Turns off the ADC
    ADCON1 = 0x0F;           // Sets all bits to digital mode
    OSCTUNE = 0b00000000;    // This sets the 8MHz as source for 31.25KHz
    OSCCON = 0b01110000; // Selects the internal 8MHz frequency uses the primary OSC as clock source
    T0CON = 0b11000111; // Enables TMR0, sets it as 8-bit and max divide giving a clock tick of 128us
    setUpTheLCD ();     // Calls the setUpTheLCD subroutine
    clearTheScreen ();
    while (1)
    {
        writeString ("Working 8Bit LCD");
        line2 ();       // Calls subroutine to move the cursor to the start of line 2 on the LCD
        lcdInfo = 0x33; // Loads the variable lcdInfo with the ASCII for the number 3
        lcdOut ();
        while (1);  // This forever loop just makes the program halt at this point and there are no instructions in this loop. 
                    // Note: this is a one line instruction and there are no instructions before the end of instruction terminator (the semi-colon))
    }
            
}