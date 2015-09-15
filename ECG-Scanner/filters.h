/*
 * filters.h
 *
 *  Created on: 15 Sep 2015
 *      Author: feynman
 */

#ifndef FILTERS_H_
#define FILTERS_H_
#include "buffer.h"

void lowPassFilter(buff * input,buff * output);
void highPassFilter(buff * input,buff * output);
void derivativeFilter(buff * input,buff * output);
void squaringFilter(buff * input,buff * output);
void movingWindowFilter(buff * input,buff * output);


#endif /* FILTERS_H_ */
