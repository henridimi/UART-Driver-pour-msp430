# UART-Driver-pour-msp430
Basic example de driver UART pour un MSP430

How to Use
---

Send a simple string with UART 0
    
```C
#define UART0 0
...
uart_init(UART0);
uart_sendString(UART0, "SENDED WITH UART");
...

```
