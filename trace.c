/********************************************************************
 * File: trace.c
 *
 * main function for the pvtrace utility.
 *
 * Author: M. Tim Jones <mtj@mtjones.com>
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <linux/types.h>

#include "symbols.h"
#include "stack.h"


int main( int argc, char *argv[] )
{
    FILE *tracef;
    char type;
    unsigned int address;
    __u64 nano_hex;

    if (argc != 2) {

        printf("Usage: pvtrace <image>\n\n");
        exit(-1);

    }

    initSymbol( argv[1] );
    stackInit();

    tracef = fopen("trace.txt", "r");

    if (tracef == NULL) {
        printf("Can't open trace.txt\n");
        exit(-1);
    }
	
    if (feof(tracef)) {
        printf("ERROR of trace.txt\n");
		exit(-2);
	}


	//main函数需要特殊处理，先记下进入时刻,以备后续计算总的运行时间。
	fscanf( tracef, "%c0x%x %llx\n", &type, &address, &nano_hex );

	if (type == 'E') {
		addSymbol( address );
		//printf(">>stackNumElems():%d\n", stackNumElems());
		if(stackNumElems() == 0){
			//main函数需要特殊处理，先记下进入时刻,以备后续计算总的运行时间。
			setFuncTime(address, nano_hex);
		}

		addCallTrace( address );
		stackPush( address, nano_hex );
	}
	

    while (!feof(tracef)) {

        fscanf( tracef, "%c0x%x %llx\n", &type, &address, &nano_hex );

        if (type == 'E') {

            /* Function Entry */

            addSymbol( address );

            addCallTrace( address );

            stackPush( address, nano_hex );

        } else if (type == 'X') {

            /* Function Exit */

            TPopItem popItem = stackPop(nano_hex);
			setFuncTime(popItem.addr, popItem.ns);
        }

    }

    emitSymbols(nano_hex);

    fclose( tracef );

    return 0;
}

