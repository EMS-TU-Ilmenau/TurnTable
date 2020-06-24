#include <avr/io.h> // for pin definitions
#include <stdint.h> // for shorter types

// constant definitions
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define FALSE 0
#define TRUE 1
#define UART_BAUD 9600UL
#define UART_UBRR (F_CPU/(8*UART_BAUD)-1) // calculate register value for baudrate for U2Xn = 1
#define UART_MAXSTRLEN 32
#define RADIX 10 // for itoa
#define CMD_ID "AX" STR(AX_ID) // axis specific commands must start with AX<id>
#define STR_TERM '\n'
#define TACHO_PIN PB2
#define SPEED_PIN PA7
#define DIR_PIN PA6
#define MAX_PWM 1000 // pwm period in us

// variables
volatile uint8_t uartStrCount = 0;
volatile uint8_t gotUSARTInterrupt = FALSE;
volatile char uartReceiveStr[UART_MAXSTRLEN] = "";
char uartSendStr[UART_MAXSTRLEN] = "";
volatile uint8_t gotTachoInterrupt = FALSE;
volatile int32_t curStepPos = 0;
volatile int16_t tarStepVel = 0;
volatile uint16_t curStepPeriod = 0;

// prototypes
void configGPIO();
void configUSART0();
void parseCommand(char* pCmd);
void sendUSARTChar(const char c);
void sendUSARTString(const char* pStr);
void configTimer1();
void configTimer2();
int16_t parseNum(const char* strP);
void sendNum(int16_t num);