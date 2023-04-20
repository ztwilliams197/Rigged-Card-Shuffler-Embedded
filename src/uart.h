#ifndef __UART_H__
#define __UART_H__

#include "stm32f0xx.h"

typedef enum RX {RX_RESET, RX_START_SHUFFLE_MCU, RX_START_SHUFFLE_SBC, IDENTIFY_SLOT, REINDEX_SLOT} rx_action;
typedef enum TX {TX_RESET, TX_START_SHUFFLE_MCU, TX_START_SHUFFLE_SBC, CAPTURE_IMAGE, TX_STRING} tx_action;

typedef struct {
	rx_action action;
	int metadata;
} rx_packet;

rx_packet translate_packet(int packet);
int build_packet0(tx_action action);
int build_packet(tx_action action, int arg);

void send_packet(USART_TypeDef *usart, int packet);
void send_string(USART_TypeDef *usart, char *str); // must be null-terminated

#endif // __UART_H__
