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
