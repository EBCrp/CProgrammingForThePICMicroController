/*
 * File:   LCD_Programming_main.c
 * Author: Ellery Cadel
 * Copied from Chapter 6
 *
 * Created on January 15, 2024
*
*/

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

// CONFIG3H-
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
#define _XTAL_FREQ 8000000
#define firstbyte   0b00110011
#define secondbyte  0b00110011
#define fourBitOp   0b00110010      // Sets the LCD for 4-bit operation instead of 8-bit
#define twoLines    0b00101100      // Sets LCD to 2 line mode
#define incPosition 0b00000110      // Tells the LCD to increment the cursor position after any data is displayed
#define cursorNoBlink   0b00001100  // Turns the cursor off so no flashing
#define clearScreen 0b00000001      // Clears the screen of all display
#define returnHome  0b00000010      // Sends cursor back to starting position on display
#define lineTwo     0b11000000      // Send the cursor to the start of line 2 on display
#define doBlink     0b00001111      // Turns the cursor on and makes it blink
#define shiftLeft   0b00010000      // Shifts cursor one position to the left
#define shiftRight  0b00010100      // Shifts cursor one position to the right
#define lcdPort     PORTB           // Sets connection port for LCD
#define eBit        PORTBbits.RB5   // Sets the bit for the E pin on the LCD
#define rspin       PORTBbits.RB4   // Sets the bit for the RS pin on the LCD

// Some variables. These comments are just to split the listing into logical sections

unsigned char lcdInfo, lcdTempInfo, rsOr;   // Reserves 3 8-bit memory locations, 1 for each variable
unsigned char n;                            // Reserves an 8-bit memory location for variable 'n'
unsigned char str[80];       // Sets up 80 memory locations one after the other in an array. This array is used for sending a string to the LCD
char lcdInitialize [8] =    // *A* Sets up 8 memory locations in an array and loads each location with some initial data in each location
{                           // Opening bracket of the array
    firstbyte,              // Note: the data in each location is one of the set up instructions for the LCD
    secondbyte,             // Just a repeat of the first byte
    fourBitOp,              // Puts the lcd into 4-bit mode not 8-bit
    twoLines,               // Set the lcd up for using two lines of 16 characters
    incPosition,            // Sets the lcd up for moving the cursor one place to the right everytime a character is displayed
    cursorNoBlink,          // Sets the lcd up to not show the cursor on the screen
    clearScreen,            // Clears all characters from the lcd display
    returnHome,             // Sends the cursor to the beginning of line 1
};                          // Closing bracket of the array. Note: the semi-colon is needed as this is the end of a program instruction
// The subroutine

void sendInfo ()            // Subroutine to send info to the lcd
{                           // Opening bracket for the sendInfo subroutine
    lcdTempInfo = (lcdTempInfo << 4 | lcdTempInfo >> 4);    // Swaps nibbles around in lcdTempInfo ready to send to the LCD
    lcdInfo = lcdTempInfo & 0x0F;   // Basically ignores the last four bits of the lcdTempInfo. 
                                    // b4, b5, b6 and b7 will always be logic '0' and loads the result into lcdInfo
    lcdInfo = lcdInfo | rsOr;       // Performs a logical OR with lcdInfo and rs0r. 
                                    // Allows us to determine if the info is an instruction or data set
    lcdPort = lcdInfo;              // Sends the info to the LCD
    eBit = 1;                       // Tells the driver that new info has arrived at the lcd
    eBit = 0;
    __delay_ms(2);
}                                   // Closing bracket for sendInfo subroutine

void lcdOut ()                      // Subroutine gets the data ready to go to the sendInfo
{                                   // Opening bracket for the lcdOut subroutine
    lcdTempInfo = lcdInfo;          // Store the information in a temporary location
    sendInfo ();                    // Calls the subroutine sendInfo to send the high nibble first
    sendInfo ();                    // Calls the subroutine sendInfo again to send the low nibble next
}                                   // Closing bracket for the lcdOut subroutine

void setUpTheLCD ()                 // Subroutine to set up the LCD
{                                   // Opening bracket for the setUpTheLCD subroutine
    __delay_ms(32);
    n=0;
    rsOr = 0x00;                    // Ensures bit 4 or the RS pin will be logic '0' as these are instructions
    while (n < 8)                   // While the value in variable 'n' is less than 8 do what is inside the curly brackets
    {                               // Opening bracket for the while instruction
        lcdInfo = lcdInitialize [n];    // Load the variable "lcdInfo" with what is in location 'n' in the lcdInitialize array
                                        // As 'n' starts out at 0 this will be the first location in the array (see *A* above)
        lcdOut ();                  // Call the subroutine lcdOut
        n++;                        // Add one to the value of 'n', then the program will go back to 'while (n<8)'
    }                               // Closing bracket for while statement
    rsOr = 0x10;                    // Ensures bit 4 of the rs0r is logic '1' for data
}                                   // Closing bracket for setUp subroutine (0x10 = 0b00010000)

void line2 ()                       // Subroutine to send the cursor of the lcd to the start of line 2
{                                   // Opening bracket of line 2 subroutine
    rsOr = 0x00;                     // Ensures bit 4 or the RS pin will be logic '0' as these are instructions
    lcdInfo = lineTwo;               // Loads the variable "lcdInfo" with the instruction to send the cursor to line 2
    lcdOut ();                      // Call the subroutine lcdOut
    rsOr = 0x10;                    // Ensures bit 4 of the rs0r is logic '1' for data
}                                   // Closing bracket for line2 subroutine

void cleartheScreen ()              // Subroutine to cleartheScreen
{                                   // Opening bracket for cleartheScreen subroutine
    rsOr = 0x00;                    // Ensures bit 4 (RS pin) will be logic '0' for instructions
    lcdInfo = clearScreen;          // Load the variable "lcdInfo" with the instruction to clear the lcd display
    lcdOut ();                      // Call the subroutine "lcdOut"
    lcdInfo = returnHome;           // Load the variable "lcdInfo" with the instruction to return the cursor to the beginning of line 1  on the lcd display
    lcdOut ();                      // Call the subroutine "lcdOut"
    rsOr = 0x10;                     // Ensures bit 4 of the rs0r is logic '1' for data
}

void writeString (const char *words)    // Subroutine that will display a whole string of characters on the display
{
    while (*words)                // While the *words pointer is not pointing to the NULL char do what is inside the curly brackets
    {                               // Opening bracket for the while loop
        lcdInfo = *words;           // Load what the *words pointer is pointing to into the variable lcdInfo
        lcdOut ();                  // Call subroutine to pass the data to the LCD
        *words++;                  // increment the contents of the pointer so that it is pointing to the next char in the array
    }                               // Closing bracket for the while loop
}                                   // Closing bracket for the writeString subroutine

void main ()
{                                   // Opening bracket for the main loop
    PORTA = 0;                      // Makes sure the PORTS don't turn anything on
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    TRISA = 0xFF;                   // Set all bits on PORTA to inputs
    TRISB = 0x00;                   // Set all bits on PORTB to outputs
    TRISC = 0x00;                   // Set all bits on PORTC to outputs
    TRISD = 0x00;                   // Set all bits on PORTD to outputs
    ADCON0 = 0b00000001;             // Channel 0 AN0 is selected
    ADCON1 = 0b00001011;            // Bits A0 to A3 are analog, balance are digital
    ADCON2 = 0b00010001;            // Select left justify, 4TAD Clock = FOSC/8 (1MHz), TOSC = 1ms, 2.4u/1m = 2.4, 4x1ms = 4ms
    OSCTUNE = 0x00;                 // Don't use any fine turning aspects for the oscillator
    OSCCON = 0x74;                  // Sets the internal oscillator to 8MHz stable
    T0CON = 0xC7;                   // Turns timer 0 on, makes it an 8-bit timer with the maximum divide rate
    setUpTheLCD ();                 // Call the subroutine to set up the LCD
    while (1)                       // Forever loop to ensure beginning of main loop carried out only once
    {                               // Opening bracket for forever (while) loop
        
        writeString ("Hello World");    // Sends the string "Hello World" to the LCD
        line2 ();                   // Call the subroutine line2 to move the cursor to the beginning of line 2 on the LCD
        lcdInfo = 0b00110101;       // This is the ASCII code for the number 5
        lcdOut ();                  // Call the subroutine to send the number out to the LCD
        lcdInfo = 0b00110111;       // This is the ASCII code for the number 7
        lcdOut ();                  // Call the subroutine to send the number out to the LCD
        lcdInfo = 0b001110001;      // This is the ASCII character for the number 9
        lcdOut ();                  // Call the subroutine to send the number out to the LCD
        while (1);                  // Another forever loop used to make the program halt at this point as the PIC will forever do nothing
    }                               // Closing bracket for the first forever loop
}                                   // Closing bracket for the main loop
