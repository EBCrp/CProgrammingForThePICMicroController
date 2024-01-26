/*
 * File:   SpecialCharProgram_Chapter_6
 * Name: Special Character Program for use with LCD
 * 
 * This is a basic program to control the LCD using the PIC18F4525
 * Written by H H Ward dated 31/10/15
 * It is for use with the matrix multimedia prototype board using 4-bit operation on PortB
 * Extended to include creating special characters 27/03/16
 * Author: Ellery Cadel
 * Copied from Chapter 6
 *
 * Created on January 18, 2024
*
*/

#include "config_HHWardBook1.h"
#include <xc.h>
#include <stdio.h>

// Some definitions
#define _XTAL_FREQ 8000000          // Oscillator frequency for delay
#define firstbyte 0b00110011        // The first instruction to be send to the LCD
#define secondbyte 0b00110011       // The second instruction to be send to the LCD
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
#define eBit PORTBbits.RB5          // Sets the bit for the E pin on the LCD
#define startButton PORTAbits.RA0   // Tells compiler the waitbutton is on bit0 of PortA

// Some variables
unsigned char lcdData, lcdTempData, rsLine; // Declare some variables as unsigned char
unsigned char n;

// The subroutines

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
    rsLine = 0x00;              // Ensures bit-4 or the RS pin will be logic '0' as these are instructions
    n=0;                        // Makes variable n = 0 ready for the while loop upcoming
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

void writeString (const char *words)
{
    while (*words)          // While the *words pointer is not pointing to the NULL char do what is inside the brackets
    {
        lcdData = *words;   // Load what the *words pointer is pointing to into the variable lcdData
        lcdOut ();          // Call the subroutine to pass the data to the LCD
        *words ++;          // Increment the contents of the pointer so that it is pointing to the next char in the array
    }
}

char firstCharacter [8] =   // Sets up an array of 8 locations and loads each location with the data to create the pixel map for the character
{
    0b00000100,
    0b00001110,
    0b00000100,
    0b00000100,
    0b00000100,
    0b00000100,
    0b00001110,
    0b00000100,
};

char secondCharacter [8] =  // Sets up an array of 8 locations and loads each location with the data to create the pixel map for the next character
{
    0b00010101,
    0b00010101,
    0b00010101,
    0b00010101,
    0b00010101,
    0b00010101,
    0b00010001,
    0b00010001,
};

char thirdCharacter [8] =
{
    0b00000001,
    0b00000001,
    0b00000001,
    0b00000011,
    0b00000011,
    0b00000011,
    0b00000011,
    0b00000011,        
};

char fourthCharacter [8] =
{
    0b00010001,
    0b00010001,
    0b00000100,
    0b00000100,
    0b00011011,
    0b00011011,
    0b00011111,
    0b00001110,
};

void clearTheScreen ()

{
   rsLine = 0x00;
   lcdData = clearScreen;
   lcdOut ();
   lcdData = returnHome;
   lcdOut ();
   rsLine = 0x10;
}

void writeToGram ()         // Subroutine to write code to the CGram location in the LCD
{
    rsLine = 0x00;          // Ready for instruction
    lcdData = 0x40;         // Tells the LCD to open the first address in CGRAM area ready for us to write data into them
    lcdOut ();
    rsLine = 0x10;          // Ready for data as we have finished sending instructions
    n=0;
    while (n < 8)
    {
        lcdData = firstCharacter [n];   // Loads lcdData with data from the array identified by the variable 'n'
        lcdOut ();                      // Calls the subroutine to start sending the information to the LCD
        n++;
    }
    n=0;
    while (n < 8)
    {
        lcdData = secondCharacter [n];
        lcdOut ();
        n ++;
    }
    n = 0;
    while (n < 8)
    {
        lcdData = thirdCharacter [n];
        lcdOut ();
        n ++;
    }
    n=0;
    while (n < 8)
    {
        lcdData = fourthCharacter [n];
        lcdOut ();
        n ++;
    }
    rsLine = 0x00;      // Get ready for instructions
    lcdData = 0x80;     // command to go to DDRAM address
    lcdOut ();
    rsLine = 0x10;
}

// Main Program

void main ()
{
    PORTA = 0;          // The following 4 instructions load '0' into the 4 ports to make sure they are not turning on anything
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    TRISA = 0xFF;               // Loads logic '1' to all bits iin TRISA thus making all PORTA inputs
    TRISB = 0x00;               // Loads logic '0' to all bits in TRISB thus making all PORTB outputs
    TRISC = 0x00;               // Loads logic '0'  to all bits in TRISC thus making all PORTC outputs
    TRISD = 0x00;               // Loads logic '0' to all bits in TRISD thus making all PORTD outputs
    ADCON0 = 0x00;              // Turns off ADC
    ADCON1 = 0x0F;               // Sets all bits to digital mode
    OSCTUNE = 0b100000000;      //This sets the 8MHz as source for 31.25KHz
    OSCCON = 0b01110100;        // This selects the internal 8MHz frequency stable uses the primary osc as clock source
    T0CON = 0b110000111;        // EnablesTMR0, sets it as 8-bit and max divide giving T812.5Hz therefore 128usec per tic 
    __delay_ms(33);
    setUpTheLCD ();             // Call the setUpTheLCD subroutine
    clearTheScreen ();          // Call the subroutine to clear the screen and send the cursor back to the start of the display
    writeToGram ();             // Call the subroutine to write the data for the special characters to the Gram of the LCD
    while (!startButton);       // Make the program wait until the start button on PORTA has been pressed and so gone to a logic '1'
    while (1)
    {
        writeString ("Special Chars");  // Calls the subroutine writeString and sends the string Special Chars to be displayed on the LCD
        lcdData = 0x31;         // Calls the subroutine writeString and sends the string Special Chars to be displayed on the LCD
        lcdOut ();              // Sends the data to the LCD
        lcdData = 0x32;         // Loads the variable lcdData with value 0x32
        lcdOut ();
        lcdData = 0x33;         // Loads the variable lcdData with value 0x33 this is the ASCII for the number 3
        lcdOut ();
        line2 ();               // Calls the subroutine to move the cursor to beginning of line 2 on the LCD display
        lcdData = 0x00;         // Loads the variable lcdData with the value 0 this is the address of the first area in the CGram of the LCD
        lcdOut ();
        lcdData = 0x01;         // Loads the variable lcdData with the value 1 this is the address of second area in the CGram of the LCD
        lcdOut ();
        lcdData = 0x02;         // Loads the variable lcdData with the value 2 this is the address of the third area in the CGram of the LCD
        lcdOut ();
        lcdData = 0x03;         // Loads the variable lcdData with the value 3 this is the address of the forth area in the CGram of the LCD
        lcdOut ();
        lcdData = 0x20;         // Loads the variable lcdData with the ASCII for the space (see Table 6-1)
        lcdOut ();
        lcdData = 0x48;         // Loads the variable lcdData with the ASCII for capital H (see Table 6-1)    
        lcdOut ();
        lcdData = 0x2E;         // Loads the variable lcdData with the ASCII for full stop (see Table 6-1)
        lcdOut ();
        lcdData = 0x57;         // Loads the variable lcdData with the ASCII for capital W (see Table 6-1)
        lcdOut ();
        lcdData = 0x2E;         // Loads the variable lcdData with the ASCII for full stop (see Table 6-1)
        lcdOut ();  
        rsLine = 0x00;          // Sets the variable rsLine to 0 ready to tell the LCD the next info is an instruction
        lcdData = returnHome;   // Loads the variable lcdData with the instruction to return the cursor back to the beginning of the LCD
        lcdOut ();
        rsLine = 0x10;          // Sets the variable rsLine to 1 ready to tell the LCD the next info is data to be displayed
    }           
    }
           