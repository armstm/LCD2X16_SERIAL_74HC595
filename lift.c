/*******************************************************
This program for serial lcd 2x 16 whit ic 74hc595

Project : ser_lcd2x16
Version : v0.0
Date    : 3/2/2022
Author  : mohammad amin
Company : javadi
Comments: mohammadamin.javadi@gmail.com


Chip type               : ATmega8A
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*******************************************************/

#include <mega8.h>
#include <delay.h>

#include <stdio.h>
#include <string.h>

#include "initialization.h"

/* Private HD44780 structure */
typedef struct {
    unsigned int DisplayControl;
    unsigned int DisplayFunction;
    unsigned int DisplayMode;
    unsigned int Rows;
    unsigned int Cols;
    unsigned int currentX;
    unsigned int currentY;
} HD44780_Options_t;

/* Rectangle for custom character */
/* xxx means doesn't care, lower 5 bits are important for LCD */
unsigned int customChar[] = {
	0x1F,	/*  xxx 11111 */
	0x11,	/*  xxx 10001 */
	0x11,	/*  xxx 10001 */
	0x11,	/*  xxx 10001 */
	0x11,	/*  xxx 10001 */
	0x11,	/*  xxx 10001 */
	0x11,	/*  xxx 10001 */
	0x1F	/*  xxx 11111 */
};


unsigned int Buffer_Data_Lcd[8];


/* Private functions */
static void TM_HD44780_InitPins(void);
static void TM_HD44780_Cmd(unsigned int cmd);
static void TM_HD44780_Cmd4bit(unsigned int cmd);
static void TM_HD44780_Data(unsigned int data);
static void TM_HD44780_CursorSet(unsigned int col, unsigned int row);
static void CMD_74595_LCD(void);

/* Private variable */
static HD44780_Options_t HD44780_Opts;

/* Pin definitions */     
#define HD44780_SH PORTC.0
#define HD44780_ST PORTC.2      
#define HD44780_DS PORTC.1 
       
#define HD44780_E_HI     Buffer_Data_Lcd[EN]=HI;  delay_ms(2); 
#define HD44780_E_LOW    Buffer_Data_Lcd[EN]=LOW; delay_ms(2);

#define HD44780_Delay(x)            delay_ms(x)

#define RS  0
#define EN  1
#define RW  2
#define DD  3
#define D4  4
#define D5  5
#define D6  6
#define D7  7

#define DATAPIN 1
#define CTRLPIN 2

/* Commands*/
#define HD44780_CLEARDISPLAY        0x01
#define HD44780_RETURNHOME          0x02
#define HD44780_ENTRYMODESET        0x04
#define HD44780_DISPLAYCONTROL      0x08
#define HD44780_CURSORSHIFT         0x10
#define HD44780_FUNCTIONSET         0x20
#define HD44780_SETCGRAMADDR        0x40
#define HD44780_SETDDRAMADDR        0x80

/* Flags for display entry mode */
#define HD44780_ENTRYRIGHT          0x00
#define HD44780_ENTRYLEFT           0x02
#define HD44780_ENTRYSHIFTINCREMENT 0x01
#define HD44780_ENTRYSHIFTDECREMENT 0x00

/* Flags for display on/off control */
#define HD44780_DISPLAYON           0x04
#define HD44780_CURSORON            0x02
#define HD44780_BLINKON             0x01

/* Flags for display/cursor shift */
#define HD44780_DISPLAYMOVE         0x08
#define HD44780_CURSORMOVE          0x00
#define HD44780_MOVERIGHT           0x04
#define HD44780_MOVELEFT            0x00

/* Flags for function set */
#define HD44780_8BITMODE            0x10
#define HD44780_4BITMODE            0x00
#define HD44780_2LINE               0x08
#define HD44780_1LINE               0x00
#define HD44780_5x10DOTS            0x04
#define HD44780_5x8DOTS             0x00

void TM_HD44780_Init(unsigned int cols, unsigned int rows) {
    /* Initialize delay */
    
    /* Init pinout */
    TM_HD44780_InitPins();
    
    /* At least 40ms */
    HD44780_Delay(4);
    
    /* Set LCD width and height */
    HD44780_Opts.Rows = rows;
    HD44780_Opts.Cols = cols;
    
    /* Set cursor pointer to beginning for LCD */
    HD44780_Opts.currentX = 0;
    HD44780_Opts.currentY = 0;
    
    HD44780_Opts.DisplayFunction = HD44780_4BITMODE | HD44780_5x8DOTS | HD44780_1LINE;
    if (rows > 1) {
        HD44780_Opts.DisplayFunction |= HD44780_2LINE;
    }
    
    /* Try to set 4bit mode */
    TM_HD44780_Cmd4bit(0x03);
    HD44780_Delay(4);
    
    /* Second try */
    TM_HD44780_Cmd4bit(0x03);
    HD44780_Delay(4);
    
    /* Third goo! */
    TM_HD44780_Cmd4bit(0x03);
    HD44780_Delay(4);   
    
    /* Set 4-bit interface */
    TM_HD44780_Cmd4bit(0x02);
    HD44780_Delay(4);
    
    /* Set # lines, font size, etc. */
    TM_HD44780_Cmd(HD44780_FUNCTIONSET | HD44780_Opts.DisplayFunction);

    /* Turn the display on with no cursor or blinking default */
    HD44780_Opts.DisplayControl = HD44780_DISPLAYON;
    TM_HD44780_DisplayOn();

    /* Clear lcd */
    TM_HD44780_Clear();

    /* Default font directions */
    HD44780_Opts.DisplayMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
    TM_HD44780_Cmd(HD44780_ENTRYMODESET | HD44780_Opts.DisplayMode);

	/* Delay */
	HD44780_Delay(4);
}

void TM_HD44780_Puts(unsigned int x, unsigned int y, char* str) {
	TM_HD44780_CursorSet(x, y);
	while (*str) {
		if (HD44780_Opts.currentX >= HD44780_Opts.Cols) {
			HD44780_Opts.currentX = 0;
			HD44780_Opts.currentY++;
			TM_HD44780_CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
		}
		if (*str == '\n') {
			HD44780_Opts.currentY++;
			TM_HD44780_CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
		} else if (*str == '\r') {
			TM_HD44780_CursorSet(0, HD44780_Opts.currentY);
		} else {
			TM_HD44780_Data(*str);
			HD44780_Opts.currentX++;
		}
		str++;
	}
}

void TM_HD44780_DisplayOn(void) {
	HD44780_Opts.DisplayControl |= HD44780_DISPLAYON;
	TM_HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void TM_HD44780_DisplayOff(void) {
	HD44780_Opts.DisplayControl &= ~HD44780_DISPLAYON;
	TM_HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void TM_HD44780_BlinkOn(void) {
	HD44780_Opts.DisplayControl |= HD44780_BLINKON;
	TM_HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void TM_HD44780_BlinkOff(void) {
	HD44780_Opts.DisplayControl &= ~HD44780_BLINKON;
	TM_HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void TM_HD44780_CursorOn(void) {
	HD44780_Opts.DisplayControl |= HD44780_CURSORON;
	TM_HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void TM_HD44780_CursorOff(void) {
	HD44780_Opts.DisplayControl &= ~HD44780_CURSORON;
	TM_HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void TM_HD44780_ScrollLeft(void) {
	TM_HD44780_Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}

void TM_HD44780_ScrollRight(void) {
	TM_HD44780_Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}

void TM_HD44780_CreateChar(unsigned int location, unsigned int *data) {
	unsigned int i;
	/* We have 8 locations available for custom characters */
	location &= 0x07;
	TM_HD44780_Cmd(HD44780_SETCGRAMADDR | (location << 3));
	
	for (i = 0; i < 8; i++) {
		TM_HD44780_Data(data[i]);
	}
}

void TM_HD44780_PutCustom(unsigned int x, unsigned int y, unsigned int location) {
	TM_HD44780_CursorSet(x, y);
	TM_HD44780_Data(location);
}

void TM_HD44780_Clear(void) {
	TM_HD44780_Cmd(HD44780_CLEARDISPLAY);
	HD44780_Delay(45);
}

/* Private functions */
static void TM_HD44780_Cmd(unsigned int cmd) {
	/* Command mode */
	Buffer_Data_Lcd[RS]=LOW; 
    CMD_74595_LCD();
	
	/* High nibble */
	TM_HD44780_Cmd4bit(cmd >> 4);
	/* Low nibble */
	TM_HD44780_Cmd4bit(cmd & 0x0F);
}

static void TM_HD44780_Data(unsigned int data) {
	/* Data mode */
	Buffer_Data_Lcd[RS]=HI;
    CMD_74595_LCD();
	
	/* High nibble */
	TM_HD44780_Cmd4bit(data >> 4);
	/* Low nibble */
	TM_HD44780_Cmd4bit(data & 0x0F);
}

static void TM_HD44780_Cmd4bit(unsigned int cmd) {
	/* Set output port */
	if((cmd & 0x08)) 
	    Buffer_Data_Lcd[D7]=1;
	else
	    Buffer_Data_Lcd[D7]=0;

	if((cmd & 0x04)) 
	    Buffer_Data_Lcd[D6]=1;
	else
	    Buffer_Data_Lcd[D6]=0;
	
	if((cmd & 0x02)) 
	    Buffer_Data_Lcd[D5]=1;
	else
	    Buffer_Data_Lcd[D5]=0;
	
	if((cmd & 0x01)) 
	    Buffer_Data_Lcd[D4]=1;
	else 
        Buffer_Data_Lcd[D4]=0;
    
    CMD_74595_LCD(); 
    
    HD44780_E_HI;  CMD_74595_LCD(); 
    HD44780_E_LOW; CMD_74595_LCD(); 
     
}

static void TM_HD44780_CursorSet(unsigned int col, unsigned int row) {
	unsigned int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	
	/* Go to beginning */
	if (row >= HD44780_Opts.Rows) {
		row = 0;
	}
	
	/* Set current column and row */
	HD44780_Opts.currentX = col;
	HD44780_Opts.currentY = row;
	
	/* Set location address */
	TM_HD44780_Cmd(HD44780_SETDDRAMADDR | (col + row_offsets[row]));
}

static void TM_HD44780_InitPins(void) {
    int i;
        
    for (i=0 ; i < 8 ; i++){  
        Buffer_Data_Lcd[i]=0x00;
    }
    CMD_74595_LCD();
}

static void CMD_74595_LCD(void){
    HD44780_ST=0; 
    HD44780_DS =Buffer_Data_Lcd[0];  HD44780_SH=0;  HD44780_SH=1;
    HD44780_DS =Buffer_Data_Lcd[1];  HD44780_SH=0;  HD44780_SH=1;
    HD44780_DS =Buffer_Data_Lcd[2];  HD44780_SH=0;  HD44780_SH=1;
    HD44780_DS =Buffer_Data_Lcd[3];  HD44780_SH=0;  HD44780_SH=1;
    HD44780_DS =Buffer_Data_Lcd[4];  HD44780_SH=0;  HD44780_SH=1;
    HD44780_DS =Buffer_Data_Lcd[5];  HD44780_SH=0;  HD44780_SH=1;
    HD44780_DS =Buffer_Data_Lcd[6];  HD44780_SH=0;  HD44780_SH=1;
    HD44780_DS =Buffer_Data_Lcd[7];  HD44780_SH=0;  HD44780_SH=1;
    HD44780_ST=1;  
}

void main(void)
{
    /* Input/Output Ports initialization */
    initialization();

    /* Initialize LCD 16 cols x 2 rows */
    TM_HD44780_Init(16,2);

    /* Save custom character on location 0 in LCD */
    TM_HD44780_CreateChar(0, customChar);	
    				
    TM_HD44780_Puts(0 , 0,"Welcome  Serial ");
    TM_HD44780_Puts(0 , 1,"LCD 2X16  >>>   "); 
    HD44780_Delay(200);
    	               				                                
while (TRUE)
      { 

      }
}
