/********************************************************************
 * File: stack.h
 *
 * Simple stack implementation header.
 *
 * Author: M. Tim Jones <mtj@mtjones.com>
 *
 */

#ifndef __STACK_H
#define __STACK_H

#include <linux/types.h>

typedef struct {
    int addr;
    __u64 ns;
} TPopItem;

void stackInit( void );

int stackNumElems( void );

unsigned int stackTop( void );

void stackPush( unsigned int value, __u64 nano_hex );


TPopItem stackPop( __u64 nano_hex );

#endif /* __STACK_H */

