/*
 * File:   VoltMeter_main.c
 * Author: Ellery Cadel
 * Copied from Chapter 6
 *
 * Created on January 18, 2024
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
#include <stdio.h>

// Some definitions
#define _XTAL_FREQ 8000000          // Oscillator frequency for delay
#define firstbyte 0b00110011
#define secondbyte 0b00110011
#define fourBitOp   0b00110010      // Sets LCD for 4-bit operation instead of 8
#define twoLines    0b00101100      // Sets the LCD to 2 line mode
#define incPosition 0b00000110      // Tells the LCD to increment the cursor position after any data is displayed
#define cursorNoBlink   0b00001100  // Turns the cursor off so we don't see it flashing
#define clearScreen 0b00000001      // Clears the screen of all display
#define returnHome  0b00000010      // Sends the cursor back to the start position on the display
#define lineTwo 0b11000000          // Sends the cursor to the start of line 2 on the display
#define doBlink 0b00001111          // Turns the cursor on and makes it blink
#define shiftLeft   0b00010000      // Shifts the cursor one position to the left
#define shiftRight  0b00010100      // Shifts the cursor one position to the right
#define lcdPort PORTB               // Sets which port the LCD is connected to
#define eBit    PORTBbits.RB5       // Sets the bit for the E pin on the LCD
#define rspin   PORTBbits.RB4       // Sets the bit for the RS pin on the LCD

// Some variables
unsigned char lcdData, lcdTempData, rsLine;
unsigned char n;
char str[80];
float sysVoltage;

// The subroutines

void initializeThePic ()
{
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    TRISA = 0xFF;
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    ADCON0 = 0b0000001;     // Bit 0 = '1' means ADC bits 5,5,3,2 = '0' means channel 0 AN0 is selected
    ADCON1 = 0b00001011;    // Bits A0 to A3 are analog rest are digital
    ADCON2 = 0b00010001;    // Select left justify 4TAD. Clock = FOSC/8 i.e. 1MHz TAD = 1us
    OSCTUNE = 0x00;
    OSCCON = 0x74;          // Sets the internal oscillator to 8Mhz stable
    T0CON = 0xC7;           // Turns timer 0 on, makes it an 8-bit timer with the maximum divide rate
}

char lcdInitialize [8] =    // Creates an array of 8 locations long and loads each location with the following data
{
    firstbyte,
    secondbyte,
    fourBitOp,
    twoLines,
    incPosition,
    cursorNoBlink,
    clearScreen,
    returnHome,
};

void sendData ()
{
    lcdTempData = (lcdTempData << 4 | lcdTempData >> 4);    // Swaps the nibbles around in lcdTempData ready to send to the LCD
    lcdData = lcdTempData & 0x0F;                           // Basically ignores the last four bits of the lcdTempData
    lcdData = lcdData | rsLine;                             // Allows us to determine if the info is an instruction or data
    lcdPort = lcdData;                                      // Send the info to the LCD
    eBit = 1;                                               // Next two instructions are to tell the LCD new data has arrived and it should deal with it
    eBit = 0;
    __delay_ms(3);           
}

void lcdOut ()
{
    lcdTempData = lcdData;      // Store the information in a temporary location
    sendData ();                // Sends the high nibble of the information to the LCD
    sendData ();                // Sends the low nibble of the information to the LCD
}

void setUpTheLCD ()
{
    __delay_ms(32);
    n=0;                        // Makes variable n = 0 ready for the while loop upcoming
    rsLine = 0x00;              // Ensures bit-4 or the RS pin will be logic '0' as these are instructions
    while (n <8)
    {
        lcdData = lcdInitialize [n];    // Load the variable lcdData with particular contents of the memory location in the array 
                                        // lcdInitiazlize the pointer is currently pointing to
        lcdOut ();                      // Send that information to the LCD
        n ++;  
    }
    rsLine = 0x10;                      // Ensures bit-4 of the rsLine is a logic 1 for data
}

void line2 ()
{
    rsLine = 0x00;                      // Ensures bit 4 or the RS pin will be logic 0 as these are instructions
    lcdData = lineTwo;
    lcdOut ();
    rsLine = 0x10;                      // Ensures bit 4 of the rsLine is a logic 1 for data
}

void clearTheScreen ()
{
    rsLine = 0x00;                      // Ensures bit 4 or the RS pin will be logic 0 as these are instructions
    lcdData = clearScreen;              // Loads the variable lcdData with the instruction to clear the screen
    lcdOut ();                          // Sends the instruction to the LCD
    lcdData = returnHome;               // Loads the variable lcdData with the instruction to return the cursor to the home position
    lcdOut ();                          // Sends the instruction to the LCD
    rsLine = 0x10;                      // Ensures bit 4 of the rsLine is a logic 1 for data
}

void gohome ()
{
    rsLine = 0x00;                      // Ensures bit 4 or the RS pin will be logic 0 as these are instructions
    lcdData = returnHome;               // Loads the variable lcdData with the instruction to return the cursor to the home position
    lcdOut ();                          // Sends the instruction to the LCD
    rsLine = 0x10;                      // Ensures bit 4 of the rsLine is a logic 1 for data
}

void writeString (const char *words)
{
    while (*words)          // While the *words pointer is not pointing to the NULL char do what is inside the brackets
    {
        lcdData = *words;   // Load what the *words pointer is pointing to into the variable lcdData
        lcdOut ();          // Call the subroutine to pass the data to the LCD
        *words ++;          // Increment the contents of the pointer so that it is pointing to the next char in the array
    }
}

void systemVoltage ()           // Starts a conversion and stores the result into a variable called sysVoltage. 
                                // Note: systemVoltage must be of type float as it will be a decimal number
{
    ADCON0bits.GODONE = 1;                                      // Starts the ADC conversion by setting bit 1 of ADCON0
    while (ADCON0bits.GODONE);                                  // Waits for bit 1 of the ADCON0 register to go to logic '0'. 
                                                                // This happens automatically when the conversion ends
    sysVoltage = (ADRESH*0.01953 + (ADRESL >> 6) * 0.0049);     // Converts the binary value from the ADC to the actual voltage
}

void displayVoltage (float dp)      // Creates a subroutine to display the voltage on the LCD
{
    sprintf(str, "%.2f", dp);       // This use of the function sprintf to display the floating point value using 2 decimal points
    writeString (str);              // Calls the writeStringsubroutine to send the value to the LCD
    writeString (" Volts");         // Calls the writeString subroutine to send the word Volts to the LCD
}

// Main Program

void main ()
{
    initializeThePic ();
    setUpTheLCD ();
    while (1)
    {
        writeString ("The Voltage is");
        line2 ();                       // Call the subroutine line2 to move the cursor to the beginning of line two on the LCD
        systemVoltage ();               // Calls the subroutine systemVoltage to go and measure the voltage
        displayVoltage (sysVoltage);    // Calls the subroutine displayVoltage to display the voltage on the LCD
        gohome ();                      // Calls the subroutine gohome to send the cursor on the LCD back to the home position
    }
}
