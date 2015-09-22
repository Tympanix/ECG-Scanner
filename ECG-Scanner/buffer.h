/*
 * buffer.h
 *
 *  Created on: 9 Sep 2015
 *      Author: feynman
 */

#ifndef BUFFER_H_
#define BUFFER_H_

typedef struct{
	unsigned int head;
	unsigned int size;
	unsigned long int inserts;
	int * data;
} buff;

void initBuffer(int size, buff * buffer);
void cleanupBuffer(buff * buffer);
void insertToBuffer(int data, buff * buffer);
void movePointerBuffer(buff * buffer);
int getPreviousBuffer(int previousN, buff * buffer);
int getHeadBuffer(buff * buffer);
int getAvgBuffer(buff * buffer);

#endif /* BUFFER_H_ */
