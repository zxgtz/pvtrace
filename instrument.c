/********************************************************************
 * File: instrument.c
 *
 * Instrumentation source -- link this with your application, and
 *  then execute to build trace data file (trace.txt).
 *
 * Author: M. Tim Jones <mtj@mtjones.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Function prototypes with attributes */
void main_constructor( void ) __attribute__ ((no_instrument_function, constructor));
void main_destructor( void ) __attribute__ ((no_instrument_function, destructor));

void __cyg_profile_func_enter(void *, void *) __attribute__ ((no_instrument_function));
void __cyg_profile_func_exit(void *, void *)  __attribute__ ((no_instrument_function));


///////////////////////////////////////////////////////////////////////////////
__attribute__ ((no_instrument_function))
long long now_ns_hex(){
    struct timespec tm;
    if(0==clock_gettime(CLOCK_MONOTONIC, &tm)){
        return ((long long)tm.tv_sec << 32) | tm.tv_nsec;
    } else {
        return 0;
    }
}
///////////////////////////////////////////////////////////////////////////////


static FILE *fp;


void main_constructor( void )
{
    fp = fopen( "trace.txt", "w" );
    if (fp == NULL) exit(-1);
}


void main_deconstructor( void )
{
    fclose( fp );
}


void __cyg_profile_func_enter( void *this, void *callsite )
{
    fprintf(fp, "E%p 0x%016llx\n", (int *)this, now_ns_hex());
}


void __cyg_profile_func_exit( void *this, void *callsite )
{
    fprintf(fp, "X%p 0x%016llx\n", (int *)this, now_ns_hex());
}

