/*
 * bufferPeak.h
 *
 *  Created on: 16 Sep 2015
 *      Author: feynman
 */

#ifndef BUFFERPEAK_H_
#define BUFFERPEAK_H_
#include "peak.h"

typedef struct{
	int head;
	int size;
	Peak * data;
} buffPeak;

void initBufferPeak(int size, buffPeak * buffer);
void cleanupBufferPeak(buffPeak * buffer);
void insertToBufferPeak(Peak data, buffPeak * buffer);
void movePointerBufferPeak(buffPeak * buffer);
//Peak getPreviousByIntFromBufferPeak(int previousN, buffPeak * buffer);
Peak getPreviousPeak(int previousN, buffPeak * buffer);
Peak getHeadPeak(buffPeak * buffer);

#endif /* BUFFERPEAK_H_ */
