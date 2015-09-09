
#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"




void initBuffer(int size, buff * buffer)
{
	buffer->data = calloc(size,sizeof(*(buffer->data)));
	buffer->head = size-1;
	buffer->size = size;

}

void cleanupBuffer(buff * buffer)
{
	free(buffer->data);
}

void insertToBuffer(int data, buff * buffer){
	movePointerBuffer(buffer);
	buffer->data[buffer->head] = data;
}

void movePointerBuffer(buff * buffer){
	buffer->head += 1;
	if(buffer->head >= buffer->size){
		buffer->head = buffer->head%buffer->size;
	}
}

int getPreviousByIntFromBuffer(int previousN, buff * buffer){
	int index = ((buffer->head+buffer->size)-(previousN%buffer->size))
			%buffer->size;
	return buffer->data[index];
}




