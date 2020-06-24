#include <avr/io.h> // for pin definitions
#include <stdint.h> // for shorter types
#include <avr/interrupt.h> // for sei(), cli() and ISR()
#include <string.h>	// for string operations like cmp and cat
#include <stdlib.h>	// for string to number conversion
#include "main.h" // for main setup


ISR(USART0_RX_vect) {
	// receiving chars via UART Rx
	char nextChar = UDR0;
	if (nextChar != STR_TERM && uartStrCount < UART_MAXSTRLEN) {
		// still getting valid chars
		uartReceiveStr[uartStrCount++] = nextChar;
	} else {
		// end of string
		uartReceiveStr[uartStrCount] = STR_TERM;
	  	uartStrCount = 0;
		gotUSARTInterrupt = TRUE;
	}
}

ISR(TIMER2_CAPT_vect) {
	// interrupt for timer2 input capture
	curStepPeriod = ICR2; // capture timer counter value
	TCNT2 = 0; // reset timer counter
	TCCR2B ^= (1 << ICES2); // toggle trigger edge
	if (tarStepVel > 0) curStepPos++;
	if (tarStepVel < 0) curStepPos--;
}

int main() {
	// configurations
	configGPIO();
	configUSART0();
	configTimer1();
	configTimer2();
	// activating interrupts
	sei();
	while(1) {
		if (gotUSARTInterrupt) {
			gotUSARTInterrupt = FALSE;
			parseCommand((char*)(uartReceiveStr));
		}
	}
	return 0;
}

void parseCommand(char* strP) {
	// search entry point: our axis with ID ("AX<ID>")
	char* cmdP = strstr(strP, CMD_ID);
	if (cmdP) {
		cmdP += 4; // skip "AX<ID>:"
		if (strncmp(cmdP, "VEL", 3) == 0) {
			// set speed (-1000...1000)
			cmdP += 3;
			tarStepVel = parseNum(cmdP);
			// set direction
			if (tarStepVel >= 0) {
				PORTA |= (1 << DIR_PIN); // forward
			} else {
				PORTA &= ~(1 << DIR_PIN); // backwards
			}
			// set pwm
			OCR1B = (uint16_t)(abs(tarStepVel));
		} else if (strncmp(cmdP, "POS", 3) == 0) {
			// current position
			cmdP += 3;
			if (*cmdP == '?') {
				sendNum(curStepPos);
			} else {
				curStepPos = parseNum(cmdP);
			}
		} else if (strncmp(cmdP, "PER?", 4) == 0) {
			// get current step period
			/* 
			NOTE: due to the prescaler of 1024 and the clock freq of 8 MHz, 
			the step period is in [us]/128. So multiply by 128 to get it in us.
			*/
			sendNum(curStepPeriod);
		} else {
			// no valid command
			strcpy(uartSendStr, "No valid command: ");
			strcat(uartSendStr, cmdP);
			sendUSARTString(uartSendStr);
		}
	} else {
		// no axis specific command
		cmdP = strstr(strP, "*IDN?");
		if (cmdP) {
			// requested for device name
			sendUSARTString(CMD_ID);
		}
	}
}

int16_t parseNum(const char* strP) {
	// parses the string for a decimal number
	char *nxt;
	return strtol(strP, &nxt, RADIX);
}

void sendNum(int16_t num) {
	// converts a decimal number to string and sends it over USART
	itoa(num, uartSendStr, RADIX);
	sendUSARTString(uartSendStr);
}

void configGPIO() {
	// configure pin i/o
	DDRA |= (1 << SPEED_PIN) | (1 << DIR_PIN); // output pins for direction and speed pwm
}

void configUSART0() {
	// set baud rate
	UBRR0H = (uint8_t)(UART_UBRR >> 8);
	UBRR0L = (uint8_t)UART_UBRR;
	// enable receiver and transmitter pins
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	// enable receiver interrupt
	UCSR0B |= (1 << RXCIE0);
	// frame format: 8N1 (is initially set anyway)
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	// double speed because the error is too high (-7%) for baud 9600 without U2X
	UCSR0A = (1 << U2X0);
}

void configTimer1() {
	// configure 16 bit timer1 as Fast PWM mode 14, resolution set by ICR1, non-inverting
	TCCR1A = (1 << WGM11) | (1 << COM1B1);
	TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS11);
	// set top / resolution value / period
	ICR1 = MAX_PWM; // in us for F_CPU = 8MHz and prescale 8 (CS11)
	// enable channel output for TOCCx Pin
	TOCPMSA1 = (1 << TOCC6S0);
	TOCPMCOE = (1 << TOCC6OE);
	// initial duty cycle on-puls length (0...ICR1) in us
	OCR1B = 0;
}

void configTimer2() {
	// configure 16 bit timer2 as input capture to get the puls width
	TCCR2B = (1 << CS22) | (1 << CS20) | (1 << ICES2); // Prescaler 1024, trigger on rising edge
	TIMSK2 = (1 << ICIE2); // enable interrupt on input
}

void sendUSARTChar(const char c) {
	// wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0)));
	// put char into buffer
	UDR0 = c;
}

void sendUSARTString(const char* pStr) {
	// send all characters in string
	while(*pStr) {
		sendUSARTChar(*pStr);
		pStr++;
	}
	// append string termination
	sendUSARTChar(STR_TERM);
}