
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
//TODO mettre ces define dans le boardIO.h afin qu'ils devienne global et ne soient pas recopier plusieurs fois
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
//static int8_t datasize,*uartdataUart,*uartdataUart1;
static int16_t (*Rx0_Callback)(uint8_t);
static int16_t (*Rx1_Callback)(uint8_t);


//static uint8_t UARTx_CTL1[] = {UART0_CTL1,UART1_CTL1} ;
//static uint8_t UARTx_BAUDRATE[] = {UART0_BAUDRATE,UART1_BAUDRATE} ;
//static uint8_t UARTx_MCTL[] = {UART0_MCTL,UART1_MCTL} ;
//static uint8_t UARTx_CTL1[] = {UART0_CTL1,UART1_CTL1} ;
//static uint8_t UARTx_ETAT[] = {UART0_ETAT,UART1_ETAT} ;
//static uint8_t UARTx_IE[] = {UART0_IE,UART1_IE} ;



/*********************************************************************
 **
 **    Private functions declarations
 **
 *********************************************************************/


/*********************************************************************
 **
 **    Private functions 
 **
 *********************************************************************/


//**************************************************************


/*********************************************************************
 **
 **    Public functions
 **
 *********************************************************************/
// TODO � Thibault pourquoi faire un init different pour l'uart 1 et 2?
//		autant initialiser les 2 uart directement il ne sert a rien de choisir
// 		quel Uart sera initialiser, je trouve plus inteligent d'initialiser
//		les 2 directement sans demander lequel sera initialiser
uint8_t uart_init(uint8_t idx)
{
	uint8_t uart_stat=0;

	    /***********************Clock en interne a 12MHz via la PLL*******************************/
		/*	 Tableau du baudrate possible avec quartz � 4MHz (datasheet p.910) ave UCOS16 = 1
	    * 	 Fr�quence  Baudre rate		UCBRx	UCBRSx	 UCBRFx
	    =>	 12,000,000 	115200 		  6 	  0 		8
	   */

	if(!idx)
	{
			// Mode reset pour la config et r�ception de tt les char, mode clock auxiliaire (SMCLK). datasheet P.915
		UART0_CTL1 = (ENABLE<<PIN_0)|(ENABLE<<PIN_5)|(ENABLE<<PIN_7);
			// Choix de la valeur du prescaler (voir MSP430x5xx datasheet P.916).
		UART0_BAUDRATE=6;	// Parti haute du UCBR0
		UART0_BAUDRATE1=0;	// Parti basse du UCBR0
			// Dans le MCTL, le bit UCOS16 = 1 pour permettre le mode surechantillon,// la PLL fait monter la frquence jusqua 12MHz
		UART0_MCTL |= (UCBRF_8)| (UCBRS_0) | (UCOS16);
		UART0_CTL1 &= ~(ENABLE<<PIN_0); // desactive le reset,l'init UART 0 est fini

			// activation des interrupts de r�ception de l'uart
	    UART0_IE |= (UCRXIE);// don't touch to tx interrupt enable FIXME datasheet P.921
	    	// retourne la valeur de l'�tat apres l'initialisation
	    uart_stat = UART0_ETAT;
	}
	else
	{
			// Mode reset pour la config et r�ception de tt les char, mode clock auxiliaire (SMCLK). datasheet P.915
		UART1_CTL1 = (ENABLE<<PIN_0)|(ENABLE<<PIN_5)|(ENABLE<<PIN_7);
			// Choix de la valeur du prescaler (voir MSP430x5xx datasheet P.916).
#if (0)	// baudrate 115200
	    UART1_BAUDRATE=6;	// Parti haute du UCBR0
	    UART1_BAUDRATE1=0;	// Parti basse du UCBR0
			// Dans le MCTL, le bit UCOS16 = 1 pour permettre le mode surechantillon,// la PLL fait monter la frquence jusqua 12MHz
		UART1_MCTL |= (UCBRF_8)| (UCBRS_0) | (UCOS16);
#else // baudrate 9600
	    UART1_BAUDRATE=78;	// Parti haute du UCBR0
	    UART1_BAUDRATE1=0;	// Parti basse du UCBR0
			// Dans le MCTL, le bit UCOS16 = 1 pour permettre le mode surechantillon,// la PLL fait monter la frquence jusqua 12MHz
		UART1_MCTL |= (UCBRF_2)| (UCBRS_0) | (UCOS16);
#endif

		UART1_CTL1 &= ~(ENABLE<<PIN_0); // desactive le reset,l'init UART 1 est fini
			// activation des interrupts de r�ception de l'uart
	   	UART1_IE |= (UCRXIE);// don't touch to tx interrupt enable FIXME datasheet P.921
	   		// retourne la valeur de l'�tat apres l'initialisation
	   	uart_stat = UART1_ETAT;
	}


	    /********************** Clock en externe a 4MHz via le quartz            ************************/
	    /*
	    UART0_CTL1 = (DISABLE<<PIN_0)|(DISABLE<<PIN_1)|(DISABLE<<PIN_2)|(DISABLE<<PIN_3)
	    			|(ENABLE<<PIN_4)|(ENABLE<<PIN_5)|(DISABLE<<PIN_6)|(DISABLE<<PIN_7);// datasheet page 915
	     */
	   /*	 Tableau du baudrate possible avec quartz � 4MHz (datasheet p.910) avec UCOS16 = 1
	    *
	    * 	 Fr�quence  Baudre rate		UCBRx	UCBRSx	 UCBRFx
	    * 	 4,000,000 	 9600  			  26 	  0 		1
	    	 4,000,000 	 19200 			  13 	  0 		0
	    =>	 4,000,000 	 115200 		  2 	  3 		2
	   */
	/*    // Choix de la valeur du prescaler (voir MSP430x5xx datasheet P.916).
	    UART0_BAUDRATE=2;	// Parti haute du UCBR0
	    UART0_BAUDRATE1=0;	// Parti basse du UCBR0
	    // afin d'avoir un UCBR0 de 2
	    // UCOS16 = 1 pour permettre le mode surechantillon,
	    UART0_MCTL = (ENABLE<<PIN_0)|(DISABLE<<PIN_1)|(DISABLE<<PIN_2)|(DISABLE<<PIN_3)
	    			 |(DISABLE<<PIN_4)|(DISABLE<<PIN_5)|(DISABLE<<PIN_6)|(ENABLE<<PIN_7);
	    UART0_MCTL |= (UCBRF_2 | UCBRS_3 | UCOS16);// UCBRF = 2; UCSBRS = 3;*/

	   // UART0_ETAT |= (ENABLE<<PIN_0);// mode echo, uniquement pour les testes, La transmission Uart est recu comme r�ception
	    //les bit de 2 � 6 de l'UART0_ETAT indique l'�rreur qui � �t� d�tecter lors de la transmission (datasheet p.917)


return uart_stat;
}

//**************************************************************
// eventuellement un petit commentaire (le principal sera dans le .h


//**************************************************************
int8_t uart_sendChar(uint8_t idx, uint8_t data)
{
	 switch (idx)
	  {
	    case 0:
	    	while( ! (UART0_IFG & 0x02));// attent que le buffer d'�mission se vide
	    	UART0_TX = data;
	      break;
	    case 1:
	    	while( ! (UART1_IFG & 0x02));// attent que le buffer d'�mission se vide
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
        		/*if(datasize)
					{
					UART0_TX = uartdataUart[0];
					uartdataUart++; // d�calle le tableau afin d'envoyer la chaine de caractere
					datasize--;
					}
					*/
            break;
        case 0:     /* Pas d'interrupt. */
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
        		/*if(datasize)
					{
					UART1_TX = uartdataUart1[0];
					uartdataUart1++; // d�calle le tableau afin d'envoyer la chaine de caractere
					datasize--;
					}*/
            break;
        case 0:     // Pas d'interrupt.
        default:
            break;
    }
}




