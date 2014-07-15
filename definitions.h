/* Global definitions*/

//----- Status bits
#ifdef PLL  // 40MHz
	#pragma config OSC = HSPLL  //	10 x 4 = 40MHZ
#else
	#pragma config OSC = HS     //	20Mhz
#endif

#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
//#pragma config PWRT = ON not compatible with ICD2
//#pragma config XINST = ON

//----- Ports definitions
#define EXAMPLE PORTAbits.RA0	// example


struct Bits{
	unsigned bit0:1;
	unsigned bit1:1;
	unsigned bit2:1;
	unsigned bit3:1;
	unsigned bit4:1;
	unsigned bit5:1;
	unsigned bit6:1;
	unsigned bit7:1;
};

struct Bits VARbits1;

//----- Variables
int i = 0; // contatore generico
int j = 0; // contatore generico
char TimerStatus;

int Timer0_200us;
int Timer0_300us;
int Timer0_49ms;
