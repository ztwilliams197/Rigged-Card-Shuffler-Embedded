#include "uart.h"

#include "stm32f0xx.h"

rx_packet translate_packet(int packet) {
	int bits[8];
	for (int i = 0; i < 8; i++) bits[i] = packet & (1 << i);

	if (bits[7]) {
		if (bits[6]) {
			// assert 0 <= .metadata < 52
			return (rx_packet) {.action=REINDEX_SLOT, .metadata=packet & 0x3f};
		} else {
			// assert 0 <= .metadata < 52
			return (rx_packet) {.action=IDENTIFY_SLOT, .metadata=packet & 0x3f};
		}
	} else {
		// assert !bits[6]
		// assert !bits[2..5]
		if (bits[1]) {
			rx_action start_action = bits[0] ? RX_START_SHUFFLE_SBC: RX_START_SHUFFLE_MCU;
			return (rx_packet) {.action=start_action, .metadata=0};
		} else {
			// assert !bits[0]
			return (rx_packet) {.action=RX_RESET, .metadata=0};
		}
	}
}

int build_packet0(tx_action action) {
	return build_packet(action, 0);
}

int build_packet(tx_action action, int arg) {
	switch (action) {
	case TX_RESET:
		return 0x00;
	case TX_START_SHUFFLE_MCU:
		return 0x02;
	case TX_START_SHUFFLE_SBC:
		return 0x03;
	case CAPTURE_IMAGE:
		return 0x40 | (arg & 0x3f);
	case TX_STRING:
		return 0x80 | (arg & 0x7f);
	}
	return ~0;
}

void send_packet(USART_TypeDef *usart, int packet) {
	while(!(usart->ISR & USART_ISR_TXE)) { }
	usart->TDR = (uint16_t) (packet & 0xff);
}

void send_string(USART_TypeDef *usart, char *str) {
	while (*str) {
		send_packet(usart, build_packet(TX_STRING, (int) *str));
		str++;
	}
	send_packet(usart, build_packet(TX_STRING, 0)); // null terminator
}
