#include <stdio.h>
#include <stdlib.h>
#include "buffer64bit.h"


void initBuffer64bit(int size, buff64bit * buffer)
{
	buffer->data = calloc(size, sizeof(*(buffer->data)));
	buffer->head = size - 1;
	buffer->size = size;
}

void cleanupBuffer64bit(buff64bit * buffer)
{
	free(buffer->data);
}

void insertToBuffer64bit(long long int data, buff64bit * buffer) {
	movePointerBuffer64bit(buffer);
	buffer->data[buffer->head] = data;
}

void movePointerBuffer64bit(buff64bit * buffer) {
	buffer->head += 1;
	if (buffer->head >= buffer->size) {
		buffer->head = buffer->head%buffer->size;
	}
}

long long int getPreviousBuffer64bit(int previousN, buff64bit * buffer) {
	//	int index = ((buffer->head+buffer->size)-(previousN%buffer->size))
	//			%buffer->size;
	int index = buffer->head - previousN;
	if (index < 0) index += buffer->size;
	return buffer->data[index];
}

long long int getHeadBuffer64bit(buff64bit * buffer) {
	return getPreviousBuffer64bit(0, buffer);
}




