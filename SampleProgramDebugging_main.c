/*
 Sample Program for Debugging, etc
 Program Name: SampleProgramDebugging_main.c
 Chapter 8 of HHWard Book 1
 Copied by: Ellery Cadel
 Date: 1/20/2024
 */

#include "config_HHWardBook1.h"
#include <xc.h>
#include <stdio.h>

unsigned char number1 = 0x0F, t, m, a, n, b;    // Creates 6 8-bit memory locations but loads number1 with 15 or 0b00001111
int y = 2;                  // Creates a 16-bit memory location; 2 8-bits one after the other called y and loads it with value of 2
int number2 = 0xFFFF, z;    // Creates 2 more 16 bit locations and loads number2 with all logic '1's
float u = 2.55;             // Creates a variable 'u' that can store decimal numbers and loads it with 2.55
unsigned int number3 = 0xFFFF;  // Creates a 16-bit number, 'number3' where all 16-bits are used for the value and loads it all logic '1's = 65535
unsigned char list [5];     // Creates an array of 5 8-bit memory locations one after the other
unsigned char *listpointer; // Creates a 16-bit memory location that can be used to store the address of a location in an array
                            // This is 16-bits as it holds an address which is 16-bits long

void main ()
{
    number1 ++;             // Adds 1 to the value in number1
    number1 = number1 + 2;  // Makes number1 = itself but with 2 added to it
    number2 = number1 - 2;  // Loads number2 with what was in number1 but takes 2 off it
    number2 = 0b1111111111110000;   // Changes the value stored in number2 to -16
    m = 0;
    a = 0;
    y = 5;
    z = ++y;                // 'y' is incremented first then z = what y has become
    z = y++;                // z equals what y was then y is incremented
    z = (unsigned char) u;  // z changes to an unsigned char to show all 8-bits which equal 251 in decimal. This is called "Casting"
    y = 7;  
    y = ~y;                 // Inverts all the bits in the variable y
    y = 7;
    z = y << 1;             // z is back as an integer and its value is what was in y but shifted left one bit, y is unchanged
    z = y >>1;              // z is what was in y but shifted right one bit but y is unchanged
                            // Note: with the shift right instruction the LSB is simply lost. With the shift left instruction the MSB is lost
    a = 0b00010011;         // Makes a = 19
    y = (a > 0) ? a : -1;   // This is a test. Is 'a' greater than 0? If the test is true then y will = a. If the test is untrue then y = -1
    y = (a == 0) ? a : -1;  // This is a test. Is 'a' equal to 0? If the test is true then y will = a. If test is untrue then y will = -1
    y = (a > 0) ? z : -1;   // this is a test. Is 'a' greater than 0? If test is true then y will = z. If the test is untrue then y will = -1
    listpointer = list;     // Loads pointer listpointer with the first location iin the list array
    *listpointer = 2;       // Loads the first location in the list array with the value 2
    listpointer ++;         // Increments the value in *listpointer. this means it will be pointing to the next location in the array list
    *listpointer = 5;       // Loads the second loation in the array list with 5
    listpointer = list;     // Loads the pointer listpointer with the address of the first location in the array list
    a = a & 0xF0;           // Forces the first 4-bits of a to logic '0' and the last four bits will only be a logic '1' 
                            // If the last four bits in 'a' are already logic '1'
                            // this is termed bit masking or bit testing, testing to see if a bit in a variable is logic '1'
    t = 5;
    if (t && 7 == t) m = 5; // this tests to see if the value of 't' is the same as 7. 
                            // If it is then m = 5 if it is not then m=9. This is called "anding" the byte
    else m= 9;              // This is the else statement for the if then else instruction. 
                            // Note: you don't always need to write the else keyword but in this case we do need the else
    n = 0b00001000;         // Loads the value 8 into variable 'n'
    if (n & 0b00001000) m = 5;
    else m = 3;             // This is a test to see if bit 3 of the variable 'n' is at a logic '1' if it is then m = 5, if it is not then m=3. this is bit "anding"
    if (n & 0b00000001) t = 4;
    else t = 2;             // This is a test to see if bit 0 of the variable 'n' is at a logic '1' if it is then t = 4, if it is not then t = 2
    n = 10;
    for (a = 0; a < 5; a ++)
    {
        *listpointer = n;   // Loads the current location that the listpointer is pointing to with the value that is in 'n'
        listpointer ++;     // Increments the pointer listpointer so that it is pointing to the next location in the array
        n = n + 2;          // Increases the value of n by 2
    }
    while (1);              // This is a forever loop that forces the micro to stop at this instruction as it will do nothing forever
}
