/*
 * receiver.h
 *
 *  Created on: 8.6.2022
 *      Author: topir
 */

#ifndef INC_RECEIVER_H_
#define INC_RECEIVER_H_

#include <stdint.h>


/* Receiver register map */
#define CMX994A_GR			0x10
#define CMX994A_GCR			0x11
#define CMX994A_RXR			0x12
#define CMX994A_RXOFFSET	0x13
#define CMX994A_LNAINT		0x14
#define CMX994A_OCR			0x15
#define CMX994A_RXGR		0x16
#define CMX994A_RXEXTOFF	0x17
#define CMX994A_VCOCR		0x25
#define CMX994A_PLLR23		0x23
#define CMX994A_PLLR24		0x24
#define CMX994A_PLLM20		0x20
#define CMX994A_PLLM21		0x21
#define CMX994A_PLLM22		0x22

/*
 * Byte to binary convert
*/
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#define TRUE 1
#define FALSE 0

/*
 * Functions:
 */
void write_register(uint8_t reg, uint8_t val);

void write_register_2(uint8_t reg, uint8_t Qval, uint8_t Ival);

uint8_t read_register(uint8_t reg);

void demod_set_vco(uint32_t freq, uint32_t pll_r_div);

uint8_t demod_is_locked();

void configureRadio();



#endif /* INC_RECEIVER_H_ */
