# LCD2X16_SERIAL_74HC595
Programing lcd2x16 whit serial 74hc595
--------------------------------------------------------
74HC595. 8-BIT SHIFT REGISTER WITH 8-BIT OUTPUT REGISTER

The 74HC595 is an high speed CMOS device. 
An eight bit shift register accpets data from the serial input (DS) on 
each positive transition of the shift register clock (SHCP). When 
asserted low the reset function ( ) sets all shift register values to 
zero and is indepent of all clocks.
Data from the input serial shift register is placed in the output register 
with a rising pulse on the storages resister clock (STCP). With the 
output enable ( E asserted low the 3-state outputs Q0-Q7 become 
active and present th
All registers capture data on rising edge and change output on the 
falling edge. If both clocks are connected together the input shift 
register is always one clock cycle ahead of the output register.
--------------------------------------------------------
LCD 2X16 :
1. Type: Character
2. Display format: 16 x 2 characters
3. Built-in controller: ST 7066 (or equivalent)
4. Duty cycle: 1/16
5. 5 x 8 dots includes cursor
6. + 5 V power supply
7. LED can be driven by pin 1, pin 2, or A and K
--------------------------------------------------------
Ports initialization :
Pin definitions 

1. #define HD44780_SH PORTC.0
2. #define HD44780_ST PORTC.2      
3. #define HD44780_DS PORTC.1 
       
