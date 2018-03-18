#ifndef __UART_H_
#define __UART_H_

/*********************************************************************
**
**   Module      :   UART
**   Description :   Configuration et utilisation du p�riph�rique uart
**   Author      :   Henri Dimi
**
**
*********************************************************************/

/********************************************************************
**
**    Includes
**
*********************************************************************/

#include <inttypes.h>
#include <boardIO.h>

/********************************************************************
**
**    Macros / typedef
**
*********************************************************************/

/********************************************************************
**
**    Public function declarations
**
*********************************************************************/

/*!******************************************************************
*	uart_init
*  initialisation des 2 p�riph�rique UART,
*  configure les 2 Uart en mode standard  115200 baud
*
*********************************************************************/
uint8_t uart_init(uint8_t idx);

/*!******************************************************************
*	uart_sendChar
*  envoie d'un seul caractere dans le buffer d'envoi de l'UART,
*  idx  : permet de s�l�ctionner l'UART souhait�, 0 ou 1
*  data : est le caract�re � envoyer
*********************************************************************/
int8_t uart_sendChar(uint8_t idx, uint8_t data);

/*!******************************************************************
*	uart_sendString
*  envoie d'une chaine de carac�re, l'enchainement de l'envoie est
*  g�rer par interrupt,
*  idx  : permet de s�l�ctionner l'UART souhait�, 0 ou 1
*  *data : est la chaine de caract�re � envoyer
*
*********************************************************************/
int8_t uart_sendString(uint8_t idx, uint8_t *data);

/*!******************************************************************
*	uart_registerRX
*  permet de sauvegarder l'addresse de callback lors d'une
*  interruption sur RX,
*  idx  : permet de s�l�ctionner l'UART souhait�, 0 ou 1
*  func : est l'addresse de la fonction qui sera utilis�
*  		  lors de l'�venement.
*********************************************************************/
int8_t uart_registerRX(uint8_t idx,int16_t (*func)(uint8_t));


#endif /* __UART_H_ */
