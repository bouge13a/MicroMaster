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

// Don't Fucking delete things!
void operator delete( void * ptr ) {
    //assert(0);
    vPortFree ( ptr );
}

void operator delete[]( void * ptr ) {
    //assert(0);
    vPortFree ( ptr );
}
