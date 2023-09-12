/********************************************************************
 * File: symbols.h
 *
 * Symbols types and prototypes file.
 *
 * Author: M. Tim Jones <mtj@mtjones.com>
 *
 */

#ifndef __SYMBOLS_H
#define __SYMBOLS_H

#include <linux/types.h>

#define MAX_FUNCTIONS       1000
#define MAX_FUNCTION_NAME   500

typedef struct {
    unsigned int address;
    char funcName[MAX_FUNCTION_NAME+1];
	__u64 ns; //NanoSecond
	__u64 nsMax;
	__u64 nsMin;
} func_t;



void initSymbol( char *imageName );

int lookupSymbol( unsigned int address );

void addSymbol( unsigned int address );

void addCallTrace( unsigned int address );

void emitSymbols( __u64 nano_hex_max );

void setFuncTime( int index, __u64 us );

#endif /* __SYMBOLS_H */
