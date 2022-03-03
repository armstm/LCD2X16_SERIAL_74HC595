#define TRUE    1
#define FALSE   0

#define HI      1
#define LOW     0


/* 4 bit mode */
/* Control pins, can be overwritten */
/* RS - Register select pin */
#ifndef HD44780_RS_PIN
#define HD44780_RS_PORT                GPIOB
#define HD44780_RS_PIN                GPIO_PIN_7
#endif
/* E - Enable pin */
#ifndef HD44780_E_PIN
#define HD44780_E_PORT                GPIOB      //GPIO_PIN_2 IS 3.3V TOLERANT
#define HD44780_E_PIN                  GPIO_PIN_5
#endif
/* Data pins */
/* D4 - Data 4 pin */
#ifndef HD44780_D4_PIN
#define HD44780_D4_PORT                GPIOB
#define HD44780_D4_PIN                GPIO_PIN_4
#endif
/* D5 - Data 5 pin */
#ifndef HD44780_D5_PIN
#define HD44780_D5_PORT                GPIOB
#define HD44780_D5_PIN                GPIO_PIN_3
#endif
/* D6 - Data 6 pin */
#ifndef HD44780_D6_PIN
#define HD44780_D6_PORT                GPIOD
#define HD44780_D6_PIN                GPIO_PIN_2
#endif
/* D7 - Data 7 pin */
#ifndef HD44780_D7_PIN
#define HD44780_D7_PORT                GPIOC
#define HD44780_D7_PIN                GPIO_PIN_12
#endif



void initialization (void);

void TM_HD44780_Init(unsigned int cols, unsigned int rows);
void TM_HD44780_DisplayOn(void);
void TM_HD44780_DisplayOff(void);
void TM_HD44780_Clear(void);
void TM_HD44780_BlinkOn(void);
void TM_HD44780_BlinkOff(void);
void TM_HD44780_CursorOn(void);
void TM_HD44780_CursorOff(void);
void TM_HD44780_ScrollLeft(void);
void TM_HD44780_ScrollRight(void);
void TM_HD44780_CreateChar(unsigned int location, unsigned int* data);
void TM_HD44780_PutCustom(unsigned int x, unsigned int y, unsigned int location);
void TM_HD44780_Puts(unsigned int x, unsigned int y, char* str);


