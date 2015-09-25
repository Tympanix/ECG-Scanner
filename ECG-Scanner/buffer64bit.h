#ifndef BUFFER64BIT_H_
#define BUFFER64BIT_H_

typedef struct {
	unsigned int head;
	unsigned int size;
	long long int * data;
} buff64bit;

void initBuffer64bit(int size, buff64bit * buffer);
void cleanupBuffer64bit(buff64bit * buffer);
void insertToBuffer64bit(long long int data, buff64bit * buffer);
void movePointerBuffer64bit(buff64bit * buffer);
long long int getPreviousBuffer64bit(int previousN, buff64bit * buffer);
long long int getHeadBuffer64bit(buff64bit * buffer);
int getAvgBuffer64bit(buff64bit * buffer);

#endif /* BUFFER64BIT_H_ */

