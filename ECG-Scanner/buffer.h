/*
 * buffer.h
 *
 *  Created on: 9 Sep 2015
 *      Author: feynman
 */

#ifndef BUFFER_H_
#define BUFFER_H_

typedef struct Buffer{
	int head;
	int size;
	int * data;
} buff;

void initBuffer(int size, buff * buffer);
void cleanupBuffer(buff * buffer);
void insertToBuffer(int data, buff * buffer);
void movePointerBuffer(buff * buffer);
int getPreviousByIntFromBuffer(int previousN, buff * buffer);
int getHead(buff * buffer);

#endif /* BUFFER_H_ */
