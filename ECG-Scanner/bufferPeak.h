#ifndef BUFFERPEAK_H_
#define BUFFERPEAK_H_
#include "peak.h"

typedef struct{
	unsigned int head;
	unsigned int size;
	unsigned long int inserts;
	Peak * data;
} buffPeak;

void initBufferPeak(int size, buffPeak * buffer);
void cleanupBufferPeak(buffPeak * buffer);
void insertToBufferPeak(Peak data, buffPeak * buffer);
void movePointerBufferPeak(buffPeak * buffer);
//Peak getPreviousByIntFromBufferPeak(int previousN, buffPeak * buffer);
Peak getPreviousPeak(int previousN, buffPeak * buffer);
Peak getHeadPeak(buffPeak * buffer);
int getAverageRRPeak(buffPeak * buffer, int goback);

#endif /* BUFFERPEAK_H_ */
