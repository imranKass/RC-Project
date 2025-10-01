

//@author Imran Kassim and Jovanny Caballero


#include "TM4C123GH6PM.h"

void delayMs(int n) {
    int i, j;
    for(i = 0; i < n; i++)
        for(j = 0; j < 3200; j++);
}

void initGPIO(void) {
    // Enable clocks for GPIOA and GPIOB
    SYSCTL->RCGCGPIO |= (1 << 0) | (1 << 1);
    while ((SYSCTL->PRGPIO & 0x03) == 0) {}

    // Configure PA2–PA6 as output (AIN1, AIN2, BIN1, BIN2, STBY)
    GPIOA->DIR |= (0x7C);
    GPIOA->DEN |= (0x7C);
   

    // Configure PB4, PB5 as alternate functions (PWM)
    GPIOB->AFSEL |= (1 << 4) | (1 << 5);
    GPIOB->PCTL &= ~0x00FF0000;
    GPIOB->PCTL |= 0x00770000;  // PB4/PB5 = T1CCP0/T1CCP1
    GPIOB->DEN |= (1 << 4) | (1 << 5);
}

void initPWM(void) {
    SYSCTL->RCGCTIMER |= (1 << 1);  // Enable Timer1
    delayMs(1);

    TIMER1->CTL = 0;                // Disable both A and B timers
    TIMER1->CFG = 0x4;              // 16-bit timer mode
    TIMER1->TAMR = 0x0A;            // Timer A: PWM, periodic down-count
    TIMER1->TBMR = 0x0A;            // Timer B: PWM, periodic down-count

    // Set PWM frequency (Period = 16000 counts = 1 ms @ 16 MHz)
    TIMER1->TAILR = 16000 - 1;      // Timer A period
    TIMER1->TBILR = 16000 - 1;      // Timer B period

    // Set duty cycle to 50%
    TIMER1->TAMATCHR = 8000;        // Timer A match value
    TIMER1->TBMATCHR = 8000;        // Timer B match value

    // Enable timers
    TIMER1->CTL |= (1 << 0) | (1 << 8);  // Enable TAEN and TBEN
}

int forward(void) {
    

    // Set motor A forward: AIN1=1, AIN2=0
    GPIOA->DATA |= (1 << 2);               // AIN1 = 1
    GPIOA->DATA &= ~(1 << 3);              // AIN2 = 0

    // Set motor B forward: BIN1=1, BIN2=0
    GPIOA->DATA |= (1 << 4);               // BIN1 = 1
    GPIOA->DATA &= ~(1 << 5);              // BIN2 = 0

    // Enable motor driver (STBY = 1)
    GPIOA->DATA |= (1 << 6);               // STBY = 1       Enables motor driver
}



void reverse(void) {
	
	GPIOA->DATA &= ~(1 << 2);     // AIN1 = 0
	GPIOA->DATA |= (1 << 3);      // AIN2 = 1
	
	GPIOA->DATA &= ~(1 << 4);     // BIN1 = 0
	GPIOA->DATA |= (1 << 5);      // BIN2 = 1
	
	GPIOA->DATA |= (1 << 6);      // STBY = 1         Enables motor driver
}


void turn_right(void) {

GPIOA->DATA &= ~(1 << 2);        //AIN1 = 1
GPIOA->DATA |= (1 << 3);       //AIN2 =0
	
	GPIOA->DATA |= (1 << 4);     // BIN1 = 0
	GPIOA->DATA &= ~(1 << 5);     // BIN2 = 0
	
	GPIOA->DATA |= (1 << 6);       // STBY = 1  Enables motor driver
}

void turn_left(void) {

GPIOA->DATA |= (1 << 2);        //AIN1 = 1
GPIOA->DATA &= ~(1 << 3);       //AIN2 =0
	
	GPIOA->DATA &= ~(1 << 4);     // BIN1 = 0
	GPIOA->DATA |= (1 << 5);     // BIN2 = 0
	
	GPIOA->DATA |= (1 << 6);       // STBY = 1    Enables motor driver
}


void UART0_Init(void)
{
	// Enable the clock to UART0 by setting the 
	// R0 bit (Bit 0) in the RCGCUART register
	SYSCTL->RCGCUART |= 0x01;
	
	// Enable the clock to Port A by setting the
	// R0 bit (Bit 0) in the RCGCGPIO register
	SYSCTL->RCGCGPIO |= 0x01;
	
	// Disable the UART0 module before configuration by clearing
	// the UARTEN bit (Bit 0) in the CTL register
	UART0->CTL &= ~0x01;
	
	// Set the baud rate by writing to the DIVINT field (Bits 15 to 0)
	// and the DIVFRAC field (Bits 5 to 0) in the IBRD and FBRD registers, respectively.
	// The integer part of the calculated constant will be written to the IBRD register,
	// while the fractional part will be written to the FBRD register.
	// N = (System Clock Frequency) / (16 * Baud Rate)
	// N = (50,000,000) / (16 * 115200) = 27.12673611 (N = 27)
	// F = ((0.12673611 * 64) + 0.5) = 8.611 (F = 8)
	UART0->IBRD = 27;
	UART0->FBRD = 8;
	
	// Configure the data word length of the UART packet to be 8 bits by 
	// writing a value of 0x3 to the WLEN field (Bits 6 to 5) in the LCRH register
	UART0->LCRH |= 0x60;
	
	// Enable the transmit and receive FIFOs by setting the FEN bit (Bit 4) in the LCRH register
	UART0->LCRH |= 0x10;
	
	// Select one stop bit to be transmitted at the end of a UART frame by
	// clearing the STP2 bit (Bit 3) in the LCRH register
	UART0->LCRH &= ~0x08;
	
	// Disable the parity bit by clearing the PEN bit (Bit 1) in the LCRH register
	UART0->LCRH &= ~0x02;
	
	// Enable the UART0 module after configuration by setting
	// the UARTEN bit (Bit 0) in the CTL register
	UART0->CTL |= 0x01;
	
	// Configure the PA1 (U0TX) and PA0 (U0RX) pins to use the alternate function
	// by setting Bits 1 to 0 in the AFSEL register
	GPIOA->AFSEL |= 0x03;
	
	// Clear the PMC1 (Bits 7 to 4) and PMC0 (Bits 3 to 0) fields in the PCTL register before configuration
	GPIOA->PCTL &= ~0x000000FF;
	
	// Configure the PA1 pin to operate as a U0TX pin by writing 0x1 to the
	// PMC1 field (Bits 7 to 4) in the PCTL register
	// The 0x1 value is derived from Table 23-5 in the TM4C123G Microcontroller Datasheet
	GPIOA->PCTL |= 0x00000010;
	
	// Configure the PA0 pin to operate as a U0RX pin by writing 0x1 to the
	// PMC0 field (Bits 3 to 0) in the PCTL register
	// The 0x1 value is derived from Table 23-5 in the TM4C123G Microcontroller Datasheet
	GPIOA->PCTL |= 0x00000001;
	
	// Enable the digital functionality for the PA1 and PA0 pins
	// by setting Bits 1 to 0 in the DEN register
	GPIOA->DEN |= 0x03;
}

char UART0_Input_Character(void)
{
	while((UART0->FR & UART0_RECEIVE_FIFO_EMPTY_BIT_MASK) != 0);
	
	return (char)(UART0->DR & 0xFF);
}

void UART0_Output_Character(char data)
{
	while((UART0->FR & UART0_TRANSMIT_FIFO_FULL_BIT_MASK) != 0);
	UART0->DR = data;
}

void UART0_Input_String(char *buffer_pointer, uint16_t buffer_size) 
{
	int length = 0;
	char character = UART0_Input_Character();
	
	while(character != UART0_CR)
	{
		if (character == UART0_BS)
		{
			if (length)
			{
				buffer_pointer--;
				length--;
				UART0_Output_Character(UART0_BS);
			}
		}
		else if(length < buffer_size)
		{
			*buffer_pointer = character;
			buffer_pointer++;
			length++;
			UART0_Output_Character(character);
		}
		character = UART0_Input_Character();
	}
	*buffer_pointer = 0;
}

void UART0_Output_String(char *pt)
{
	while(*pt)
	{
		UART0_Output_Character(*pt);
		pt++;
	}
}

uint32_t UART0_Input_Unsigned_Decimal(void)
{
	uint32_t number = 0;
	uint32_t length = 0;
  char character = UART0_Input_Character();
	
	// Accepts until <enter> is typed
	// The next line checks that the input is a digit, 0-9.
	// If the character is not 0-9, it is ignored and not echoed
  while(character != UART0_CR)
	{ 
		if ((character >= '0') && (character <= '9')) 
		{
			// "number" will overflow if it is above 4,294,967,295
			number = (10 * number) + (character - '0');
			length++;
			UART0_Output_Character(character);
		}
		
		// If the input is a backspace, then the return number is
		// changed and a backspace is outputted to the screen
		else if ((character == UART0_BS) && length)
		{
			number /= 10;
			length--;
			UART0_Output_Character(character);
		}
		
		character = UART0_Input_Character();
	}
	
	return number;
}

void UART0_Output_Unsigned_Decimal(int n)
{
	// Use recursion to convert decimal number
	// of unspecified length as an ASCII string
  if (n >= 10)
	{
    UART0_Output_Unsigned_Decimal(n / 10);
    n = n % 10;
  }
	
	// n is between 0 and 9
  UART0_Output_Character(n + '0');
}

uint32_t UART0_Input_Unsigned_Hexadecimal(void)
{
	uint32_t number = 0;
	uint32_t digit = 0;
	uint32_t length = 0;
  char character = UART0_Input_Character();
	
	while(character != UART0_CR)
	{
		// Initialize digit and assume that the hexadecimal character is invalid
		digit = 0x10;
		
		if ((character >= '0') && (character <= '9'))
		{
			digit = character - '0';
		}
		else if ((character>='A') && (character <= 'F'))
		{
			digit = (character - 'A') + 0xA;
		}
		else if ((character >= 'a') && (character <= 'f'))
		{
			digit = (character - 'a') + 0xA;
		}
	
		// If the character is not 0-9 or A-F, it is ignored and not echoed
    if (digit <= 0xF)
		{
      number = (number * 0x10) + digit;
      length++;
      UART0_Output_Character(character);
    }
		
		// Backspace outputted and return value changed if a backspace is inputted
		else if((character == UART0_BS) && length)
		{
			number /= 0x10;
			length--;
			UART0_Output_Character(character);
		}
		
		character = UART0_Input_Character();
  }
  return number;
}

void UART0_Output_Unsigned_Hexadecimal(uint32_t number)
{
	// Use recursion to convert the number of
	// unspecified length as an ASCII string
	if (number >= 0x10)
	{
		UART0_Output_Unsigned_Hexadecimal(number / 0x10);
		UART0_Output_Unsigned_Hexadecimal(number % 0x10);
	}
	else
	{
		if (number < 0xA)
		{
			UART0_Output_Character(number + '0');
		}
		else
		{
			UART0_Output_Character((number - 0x0A) + 'A');
		}
	}
}

void UART0_Output_Newline(void)
{
	UART0_Output_Character(UART0_CR);
	UART0_Output_Character(UART0_LF);
}



// Declare pointer to the user-defined task
void (*UART1_Task)(void);

void UART_BLE_Init(void)
{
	// Enable the clock to UART1 by setting the 
	// R1 bit (Bit 1) in the RCGCUART register
	SYSCTL->RCGCUART |= 0x02;
	
	// Enable the clock to Port B by setting the
	// R10 bit (Bit 1) in the RCGCGPIO register
	SYSCTL->RCGCGPIO |= 0x02;
	
	// Disable the UART1 module before configuration by clearing
	// the UARTEN bit (Bit 0) in the CTL register
	UART1->CTL &= ~0x01;
	
	// Set the baud rate by writing to the DIVINT field (Bits 15 to 0)
	// and the DIVFRAC field (Bits 15 to 0) in the IBRD and FBRD registers, respectively.
	// The integer part of the calculated constant will be written to the IBRD register,
	// while the fractional part will be written to the FBRD register.
	// N = (System Clock Frequency) / (16 * Baud Rate)
	// N = (50,000,000) / (16 * 9600) = 325.5208333 (N = 325)
	// F = ((0.5208333 * 64) + 0.5) = 33.8333312 (F = 33)
	UART1->IBRD = 325;
	UART1->FBRD = 33;
	
	// Configure the data word length of the UART packet to be 8 bits by 
	// writing a value of 0x3 to the WLEN field (Bits 6 to 5) in the LCRH register
	UART1->LCRH |= 0x60;
	
	// Enable the transmit and receive FIFOs by setting the FEN bit (Bit 4) in the LCRH register
	UART1->LCRH |= 0x10;
	
	// Select one stop bit to be transmitted at the end of a UART frame by
	// clearing the STP2 bit (Bit 3) in the LCRH register
	UART1->LCRH &= ~0x08;
	
	// Disable the parity bit by clearing the PEN bit (Bit 1) in the LCRH register
	UART1->LCRH &= ~0x02;
	
	// Enable the UART1 module after configuration by setting
	// the UARTEN bit (Bit 0) in the CTL register
	UART1->CTL |= 0x01;
	
	// Configure the PB1 (U1TX) and PB0 (U1RX) pins to use the alternate function
	// by setting Bits 1 to 0 in the AFSEL register
	GPIOB->AFSEL |= 0x03;
	
	// Clear the PMC1 (Bits 7 to 4) and PMC0 (Bits 3 to 0) fields in the PCTL register before configuration
	GPIOB->PCTL &= ~0x000000FF;
	
	// Configure the PB1 pin to operate as a U1TX pin by writing 0x1 to the
	// PMC1 field (Bits 7 to 4) in the PCTL register
	// The 0x1 value is derived from Table 23-5 in the TM4C123G Microcontroller Datasheet
	GPIOB->PCTL |= 0x00000010;
	
	// Configure the PB0 pin to operate as a U1RX pin by writing 0x1 to the
	// PMC0 field (Bits 3 to 0) in the PCTL register
	// The 0x1 value is derived from Table 23-5 in the TM4C123G Microcontroller Datasheet
	GPIOB->PCTL |= 0x00000001;
	
	// Enable the digital functionality for the PB1 and PB0 pins
	// by setting Bits 1 to 0 in the DEN register
	GPIOB->DEN |= 0x03;
}

char UART_BLE_Input_Character(void)
{
	while((UART1->FR & UART1_RECEIVE_FIFO_EMPTY_BIT_MASK) != 0);
	
	return (char)(UART1->DR & 0xFF);
}

void UART_BLE_Output_Character(char data)
{
	while((UART1->FR & UART1_TRANSMIT_FIFO_FULL_BIT_MASK) != 0);
	UART1->DR = data;
}


int UART_BLE_Input_String(char *buffer_pointer) 
{
	int length = 0;
	int string_size = 0;
	
	char character = UART_BLE_Input_Character();
	
	while(length < 4)
	{
			*buffer_pointer = character;
			buffer_pointer++;
			length++;
			string_size++;
		
		character = UART_BLE_Input_Character();
	}
	*buffer_pointer = 0;
	
	return string_size;
}

void UART_BLE_Output_String(char *pt)
{
	while(*pt)
	{
		UART_BLE_Output_Character(*pt);
		pt++;
	}
}

uint8_t Check_UART_BLE_Data(char UART_BLE_Buffer[], char *data_string)
{
	if (strstr(UART_BLE_Buffer, data_string) != NULL)
	{
		return 0x01;
	}
	
	else
	{
		return 0x00;
	}
}

int main(void) {
	initGPIO();
	initPWM();
	
    while (1) {
			forward();
			delayMs(2000);           // Run forward for 2 sec
			
			turn_right();
			delayMs(4000);             // turn right for 4 sec
			
			turn_left();
			delayMs(4000);             // turn left for 4 sec
			
			reverse();
			delayMs(2000);            // Run reverse for 2 sec
        
    }
}


