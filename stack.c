/********************************************************************
 * File: stack.c
 *
 * Simple stack implementation.
 *
 * Author: M. Tim Jones <mtj@mtjones.com>
 *
 */

#include <assert.h>
#include <stdio.h>
#include "stack.h"

#define MAX_ELEMENTS    50

typedef struct {
	int value;
	__u64 ns; //NanoSeconds
} TStackItem;

//static int stack[MAX_ELEMENTS];
static TStackItem stack[MAX_ELEMENTS];
static int index;

void stackInit( void )
{
    index = 0;

    return;
}


int stackNumElems( void )
{
    return index;
}


unsigned int stackTop( void )
{
    assert( index > 0 );

    return (stack[index-1].value);
}


void stackPush( unsigned int value, __u64 nano_hex )
{
	//printf("push [0x%x] 0x%llx\n", value, nano_hex);
    assert ( index < MAX_ELEMENTS );

    stack[index].value = value;
	//(高8字节存放秒数；低8字节存放纳秒)==>纳秒；
    stack[index].ns = (nano_hex >> 32) * 1000000000 + (nano_hex & 0xffffffff);
	//printf("push        [%d] ns=%lld\n", index, stack[index].ns);

    index++;

    return;
}


TPopItem stackPop( __u64 nano_hex )
{

    assert( index > 0 );
    index--;

	//(高8字节存放秒数；低8字节存放纳秒)==>纳秒；
    unsigned long long nsPop = (nano_hex >> 32) * 1000000000 + (nano_hex & 0xffffffff);

    unsigned int value = stack[index].value;
	//printf("pop  [0x%x] 0x%llx\n", value, nano_hex);

	TPopItem item;
	item.addr = value;
	item.ns = (nsPop > stack[index].ns) ? nsPop-stack[index].ns : 0;
	//printf("pop         [%d] df=%llu\n", index, item.ns);

    return item;
}

