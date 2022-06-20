#include "SEGGER_RTT.h"
#include "main.h"
void readRTT(char* p);
void writeRTT(char* p);

void array2RTTbuffer(int16_t *array, int16_t size);

void prvDebugRTT(void* pvParameters);
void pulseLED(int ms_duration, int ms_pause);

void printData(int16_t *array, int16_t size);

