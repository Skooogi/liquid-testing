/*
 * crc.c
 *
 */

#include "crc.h"

/* Calculates the CRC-16 for given data of length len  */
uint32_t prvCRC16( const uint8_t* data, uint32_t len )
{
	uint16_t crc = 0x0000;										// Init value for CRC-CCITT (See "CRC-16/KERMIT" at https://reveng.sourceforge.io/crc-catalogue/all.htm)
	while (len-- > 0)
	{
		uint16_t idx = (crc ^ *(data++)) & 0xFF;				// Calculate lookup index
		crc = ((crc >> 8) & 0xFF) ^ crc16_table[idx];			// Lookup from CRC table at calculated index
	}
	return crc;													// No inversion or xor needed for CRC-CCITT TODO: Confirm this if CRC check is not working
}

