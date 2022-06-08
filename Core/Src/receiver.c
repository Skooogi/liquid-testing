/*
 * receiver.c
 *
 * This file contains the functions required to communicate and control with the CMX994 radio receiver chip.
 * The function that should mainly be used is 'configureRadio()' as it will tune the radio receiver to 162MHz
 * which is the center carrier frequency for AIS signals.
 *
 */


#include <stdio.h>
#include "receiver.h"
#include "main.h"


// Buffer to hold SPI response from CMX994 receiver
char spi_buf[20];
// LO frequency (Set to 2x center frequency and use 2 as divider for PLL as a registry setting)
uint32_t LO_FREQ = 324e6;
// Make sure this value is used in the external filter design of the VCO !!
uint32_t RVAL = 5000;


/*
 * Functions:
 */

/*
 * Writes CMX register, addresses and values defined in main.h, refer to CMX994A datasheet
 */
void write_register(uint8_t reg, uint8_t val) {

	GPIOW(DEMOD_CS, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&reg, 1, 100);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&val, 1, 100);
	GPIOW(DEMOD_CS, GPIO_PIN_SET);

}
/*
 * Required for writing the 2-byte long register in CMX994E for I and Q offset voltages
 */
void write_register_2(uint8_t reg, uint8_t Qval, uint8_t Ival) {

	GPIOW(DEMOD_CS, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&reg, 1, 100);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&Qval, 1, 100);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&Ival, 1, 100);
	GPIOW(DEMOD_CS, GPIO_PIN_SET);

}

/*
 * Reads back CMX registers and prints them in binary through RTT
 */
uint8_t read_register(uint8_t reg) {
	reg ^= 0xF0;
	GPIOW(DEMOD_CS, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&reg, 1, 100);
	HAL_SPI_Receive(&hspi2, (uint8_t *)spi_buf, 1, 100);
	GPIOW(DEMOD_CS, GPIO_PIN_SET);
	printf("Register Address: %02x: read %02x = "BYTE_TO_BINARY_PATTERN"\n", reg, spi_buf[0], BYTE_TO_BINARY(spi_buf[0]));
	return spi_buf[0];
}

/*
 * Sets the VCO frequency to the given input frequency by calculating and writing correct values to CMX register
 */
void demod_set_vco(uint32_t freq, uint32_t pll_r_div) { // TODO: Clean up



	uint32_t pll_r = pll_r_div;
	uint32_t pll_m = freq/10e6*pll_r_div; // uint32_t pll_m = freq / demod_pll_step;
	const uint8_t pll_r_23 = pll_r & 0xFF;
	const uint8_t pll_r_24 = (pll_r >> 8) & 0xFF;
	// 0x23 has to be written before 0x24
	write_register(CMX994A_PLLR23, pll_r_23);
	write_register(CMX994A_PLLR24, pll_r_24);
	//printf("Set R register: $23: %02x, $24: %02x\n", pll_r_23, pll_r_24);
	//read_register(CMX994A_PLLR23);
	//read_register(CMX994A_PLLR24);
	const uint8_t pll_m_20 = pll_m & 0xFF;
	const uint8_t pll_m_21 = (pll_m >> 8) & 0xFF;
	const uint8_t pll_m_22 = ((pll_m >> 16) & 0x07) | (1<<7) | (1<<5); // b5 = charge pump enable , b7 = Enable PLL
	write_register(CMX994A_PLLM20, pll_m_20);
	write_register(CMX994A_PLLM21, pll_m_21);
	write_register(CMX994A_PLLM22, pll_m_22);
	//printf("Set VCO: $20: %02x, $21: %02x, $22: %02x\n", pll_m_20, pll_m_21, pll_m_22);
	//read_register(CMX994A_PLLM20);
	//read_register(CMX994A_PLLM21);
	//read_register(CMX994A_PLLM22);
}

/*
 * Check from register if VCO is frequency locked
 */
uint8_t demod_is_locked(){
	return ( read_register(CMX994A_PLLM22) & (1<<6)) ? 0 : 1;
}

void configureRadio(){
	/*
	 * Writing CMX registry values
	 * CMX registry values are defined in main.h
	 * write_register() needs the register you want to write to, followed by bits you want to write 1 to
	 * Consult CMX datasheet for registry definitions
	 */
	//General reset


	/* First Enable The Radio and give LDO time to settle */
	GPIOW(EN_3V0, 1);
	HAL_Delay(200);

	/* Define all the registry values */
	uint8_t CMX994A_GCR_T = 0b10101111;
	/* General control register
	 * 	7 - EN BIAS			1
	 * 	6,5 - Freq2,Freq1 	01 for 150 MHz - 300 MHz)
	 * 	4 - LOW POWER MODE	0
	 * 	3 - VCO Enable		1 (Register $25 takes effect)
	 * 	2 - PLL Enable		1
	 * 	1 - CBUS RX Enable	1 (ORred with GPIO RX Enable pin)
	 * 	0 - CBUS TX Enable  1 (Enables TX divider) (ORred with GPIO TX Enable pin)
	 * 	*/

	uint8_t CMX994A_RXR_T = 0b00000000;
	/* RX Control Register
	 * 7 - Disable Mixer	0
	 * 6 - Disable IQ Power	0
	 * 5 - Disable LNA		0
	 * 4,3 - ACR Filter		00 for minimum BW , 10 for maximum BW
	 * 2 - DC Correction X	0 (0 for nominal , 1 for double)
	 * 1,0 - LO Divider		00 (divided by 2)
	 * */
	uint8_t CMX994A_OCR_T = 0b00000011;
	/* Options Control Register
	 * 7 - Enhanced mixer intermodulation mode (0) (only in E version)
	 * 6-4 - ----			0
	 * 3 - Q path disable	0
	 * 2 - I path disable 	0
	 * 1,0 - LO phase corr	00 (enabled 00 , powered down 11)
	 * */

	uint8_t CMX994A_VCOCR_T = 0b00000011;
	/* VCO Control Register
	 * 7 - Filter Cal Disab	0
	 * 6,5 - LO Divide TX	00 (divide by 2 = 00, 11 = no division)
	 * 4 - Enable LO in		0
	 * 3,2 - NR phase perf	11 (11 = NR miniumum with lowest Q value tank , 00 = NR Maximum with high Q tank)
	 * 1 - Enable VCO Amp	1
	 * 0 - Enable VCO Buff	1
	 */


	/* RX Offset register
	 * Recommended value from datasheet : 0x88
	 */
	// Experimentally found values for voltage offsets - the offset should be < 25 mV between each channels P and N.
	uint8_t CMX994A_RXOFFI_T = 0b00001001;
	uint8_t CMX994A_RXOFFQ_T = 0b00101010;
	//uint8_t CMX994A_RXEXT1_T = 0b10000000;
	//uint8_t CMX994A_RXEXT2_T = 0b10000000;

	/* LNA Intermodulation Control Register
	 * Recommended value from datasheet (440 MHz : 0x3F , 100 MHz : 0x00 )
	 * Could be set to 0, as very low powers are expected at input
	 */
	uint8_t CMX994A_LNAIM_T = 0x10;

	/*
	 *  Write registers with values from above
	 */
	//printf("General reset\n");
	write_register(CMX994A_GR, 0);

	//RX Control Register config
	write_register(CMX994A_RXR, CMX994A_RXR_T);
	printf("RX Control Register configured: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(CMX994A_RXR_T));
	printf("RX Control Register read:  \n  ");
	read_register(CMX994A_RXR);

	//Options Control Register config
	write_register(CMX994A_OCR, CMX994A_OCR_T);
	printf("Options Control Register configured: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(CMX994A_OCR_T));
	printf("Options Control Register read:  \n  ");
	read_register(CMX994A_OCR);

	//VCO Control Register config
	write_register(CMX994A_VCOCR, CMX994A_VCOCR_T);//CMX994A_VCOCR_TXDIV1
	printf("VCO Control Register configured: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(CMX994A_VCOCR_T));
	printf("VCO Control Register read:  \n  ");
	read_register(CMX994A_VCOCR);


	//VCO Control Register config
	write_register(CMX994A_LNAINT, CMX994A_LNAIM_T);
	write_register_2(CMX994A_RXEXTOFF, CMX994A_RXOFFQ_T, CMX994A_RXOFFI_T);


	//RX & TX on --(Unnecessary when already enabled in registry settings)
	//HAL_GPIO_WritePin (DEMOD_TXEN_GPIO_Port, DEMOD_TXEN_Pin, GPIO_PIN_SET);
	//HAL_GPIO_WritePin (DEMOD_RXEN_GPIO_Port, DEMOD_RXEN_Pin, GPIO_PIN_SET);


	/* Write the VCO registers (R and M dividers for the VCO. M is calculated from LO_FREQ and RVAL) */
	demod_set_vco(LO_FREQ, RVAL);

	//General Control Register config (Write this register last as it will enable the VCO)
	write_register(CMX994A_GCR, CMX994A_GCR_T);

	printf("General Control Register configured: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(CMX994A_GCR_T));
	printf("General Control Register read: \n  ");
	read_register(CMX994A_GCR);

	HAL_Delay(10);

	printf("Radio: PLL locked (1/0): %d \n", demod_is_locked());
	HAL_Delay(1000);


	/*
	* FOLLOWING FUNCTION IS FOR TESTING PLL MINIMUM AND MAXIMUM FREQUENCY - ADJUST CAPACITANCE/INDUCTANCE ACCORDINGLY
	*
	* Use a spectrum analyzer to see LO, and see it change between min and max frequency every 5 seconds.
	* Reduce C if frequency is too low and vice versa.
	* Check that the dividers are OK.
	* If the LO output is not a sharp, near delta function, the inductor might not have a high enough Q. The inductor should be close to the input pins.
	* Use wirewound incutor. If frequency is way off, you might need to change the inductor too.
	*/

	#if 0
		  for(int i = 0 ; i < 20; i++){
			  demod_set_vco(500e6, RVAL);
			  HAL_Delay(5000);
			  demod_set_vco(200e6, RVAL);
			  HAL_Delay(5000);
		  }

	#endif
}


