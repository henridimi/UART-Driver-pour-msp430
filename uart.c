
/*********************************************************************
 **
 **   Module      :   template
 **   Description :   blablabla
 **   Author      :   le coupable (this is HD10)
 **   Date        :   10/04/2012
 **
 **
 *********************************************************************/

/*********************************************************************
 **
 **    Includes
 **
 *********************************************************************/
#include <stdio.h>
#include <msp430.h>
#include <boardIO.h>
#include "uart.h"
#include <inttypes.h>

/*********************************************************************
 **
 **    Macros / typedef
 **
 *********************************************************************/
#define IE       UCA0IE
#define PIN_0	(0)
#define PIN_1	(1)
#define PIN_2	(2)
#define PIN_3	(3)
#define PIN_4	(4)
#define PIN_5	(5)
#define PIN_6	(6)
#define PIN_7	(7)

#define DIR_IN	(0)
#define DIR_OUT	(1)
#define OSEF	(0) // don't care

// set not connected pins (NC) cf User Guide p411
#define NC_DIR	(DIR_OUT) // output
#define NC_REN	(OSEF) // don't care
#define NC_OUT	(OSEF) // don't care
#define ENABLE	1
#define DISABLE 0

#define UART0_CTL1        UCA0CTL1
#define UART1_CTL1        UCA1CTL1
#define UART0_BAUDRATE    UCA0BR0
#define UART0_BAUDRATE1   UCA0BR1
#define UART1_BAUDRATE    UCA1BR0
#define UART1_BAUDRATE1   UCA1BR1
#define UART0_MCTL        UCA0MCTL
#define UART1_MCTL        UCA1MCTL
#define UART0_ETAT		  UCA0STAT
#define UART1_ETAT		  UCA1STAT
#define UART0_IE		  UCA0IE
#define UART1_IE		  UCA1IE
#define UART0_IFG		  UCA0IFG
#define UART1_IFG		  UCA1IFG
#define	UART0_TX		  UCA0TXBUF
#define	UART1_TX		  UCA1TXBUF
#define UART0_VEC 		  USCI_A0_VECTOR
#define UART1_VEC    	  USCI_A1_VECTOR
#define UART0_RX		  UCA0RXBUF
#define UART1_RX		  UCA1RXBUF

/*********************************************************************
 **
 **    Static variables
 **
 *********************************************************************/
static int8_t datasize, *uartdataUart, *uartdataUart1;
static int16_t (*Rx0_Callback)(uint8_t);
static int16_t (*Rx1_Callback)(uint8_t);

/*********************************************************************
 **
 **    Public functions
 **
 *********************************************************************/
uint8_t uart_init(uint8_t idx)
{
	uint8_t uart_stat=0;
	
	// Internal clock at 12MHz with PLL (datasheet p.910)
	if(!idx) 
	{
		// Reset, SMCLK : ON. datasheet P.915
		UART0_CTL1 = (ENABLE<<PIN_0)|(ENABLE<<PIN_5)|(ENABLE<<PIN_7);
		UART0_BAUDRATE=6;
		UART0_BAUDRATE1=0;
		// PLL ON 12MHz
		UART0_MCTL |= (UCBRF_8)| (UCBRS_0) | (UCOS16);
		// Enable UART0
		UART0_CTL1 &= ~(ENABLE<<PIN_0);
		// Setting of interrupt
	    UART0_IE |= (UCRXIE);
	    uart_stat = UART0_ETAT;
	} 
	
	else
	{
		UART1_CTL1 = (ENABLE<<PIN_0)|(ENABLE<<PIN_5)|(ENABLE<<PIN_7);
		UART1_CTL1 &= ~(ENABLE<<PIN_0); 
		// IE enable
	   	UART1_IE |= (UCRXIE);
	   	uart_stat = UART1_ETAT;
	}

	   /*   External clock baudrate config with 4 Mhz 
	    *   (datasheet p.910) avec UCOS16 = 1
	    * 	 Fréquence  Baudre rate		UCBRx	UCBRSx	 UCBRFx
	    * 	 4,000,000 	 9600  			  26 	  0 		1
	    	 4,000,000 	 19200 			  13 	  0 		0
	    =>	 4,000,000 	 115200 		  2 	  3 		2
	    */

	return uart_stat;
}

int8_t uart_sendChar(uint8_t idx, uint8_t data)
{
	 switch (idx)
	  {
	    case 0:
	    	while( ! (UART0_IFG & 0x02));
	    	UART0_TX = data;
	      break;
	    case 1:
	    	while( ! (UART1_IFG & 0x02));
	    	UART1_TX = data;
	      break;

	    default:
	      break;
	  }

	return 0;
}

int8_t uart_sendString(uint8_t idx, uint8_t *data)
{
	  uint16_t i = 0;

	  while(data[i])
	  	  {
		  uart_sendChar(idx,data[i]);
		  i++;
	  	  }
	  return 0;
}

int8_t uart_registerRX(uint8_t idx,int16_t (*func)(uint8_t))
{
	 switch (idx)
		  {
		    case 0:
		    	Rx0_Callback = func;
		      break;
		    case 1:
		    	Rx1_Callback = func;
		      break;

		    default:
		      break;
		  }
    return 0;
}
//**************************************************************

#pragma vector=UART0_VEC // msp430f5528.h
__interrupt void UART_ISR(void)
{
	    switch(__even_in_range(UCA0IV,4))
	    {
        case 2:     /* RX interrupt. */
      		Rx0_Callback(UART0_RX);
            break;
        case 4:     /* TX interrupt. */
        		if(datasize)
				{
					UART0_TX = uartdataUart[0];
					uartdataUart++; // clean buffer
					datasize--;
				}
					
            break;
        case 0:     /* No interrupt. */
        default:
            break;
    }
}

//**************************************************************
#pragma vector=UART1_VEC // msp430f5528.h
__interrupt void UART1_ISR(void)
{
	    switch(__even_in_range(UCA1IV,4))
	    {
        case 2:     // RX interrupt.
      			Rx1_Callback(UART1_RX);
            break;
        case 4:     // TX interrupt.
        		if(datasize)
				{
					UART1_TX = uartdataUart1[0];
					uartdataUart1++; // clean buffer
					datasize--;
				}
            break;
        case 0:     // no interrupt.
        default:
            break;
    }
}





