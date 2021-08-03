/*
 * memory_functions.cpp
 *
 *  Created on: Jul 26, 2021
 *      Author: steph
 */

#include "FreeRTOS.h"
#include <assert.h>

void* operator new(size_t size) {
    return pvPortMalloc(size);
}

void* operator new[]( size_t size ){
    return pvPortMalloc(size);
}

void operator delete( void * ptr ) {
    vPortFree ( ptr );
}

void operator delete[]( void * ptr ) {
    vPortFree ( ptr );
}
