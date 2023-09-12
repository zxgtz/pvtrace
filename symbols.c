/********************************************************************
 * File: symbols.c
 *
 * Symbols functions.  This file has functions for symbols mgmt
 *  (such as translating addresses to function names with 
 *  addr2line) and also connectivity matrix functions to keep
 *  the function call trace counts.
 *
 * Author: M. Tim Jones <mtj@mtjones.com>
 *
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "stack.h"
#include "symbols.h"

func_t       functions[MAX_FUNCTIONS];
unsigned int totals[MAX_FUNCTIONS];
unsigned int calls[MAX_FUNCTIONS][MAX_FUNCTIONS];

char imageName[50];

void initSymbol( char *image )
{
    int from, to;

    strcpy( imageName, image );

    for ( from = 0 ; from < MAX_FUNCTIONS ; from++ ) {

        functions[from].address = 0;
        functions[from].funcName[0] = 0;
        functions[from].ns = 0;
        functions[from].nsMax = 0;
        functions[from].nsMin = 0;
        totals[from] = 0;

        for ( to = 0 ; to < MAX_FUNCTIONS ; to++ ) {
            calls[from][to] = 0;
        }

    }

    return;
}


int lookupSymbol( unsigned int address )
{
    int index;

    for (index = 0 ; index < MAX_FUNCTIONS ; index++) {

        if (functions[index].address == 0) break;

        if (functions[index].address == address) return index;

    }

    assert(0);

    return 0;
}


int translateFunctionFromSymbol( unsigned int address, char *func )
{
    FILE *p;
    char line[500];
    int  len, i;

    //delete parameters & return :
    sprintf( line, "addr2line -e %s -C -f -s 0x%x | sed 's/(.*$//' | sed 's/[^ ]* //' ", imageName, address );

    p = popen( line, "r" );

    if (p == NULL) return 0;
    else {

        len = fread( line, 99, 1, p );

        i = 0;
        while ( i < strlen(line) ) {

            if ((line[i] == 0x0d) || (line[i] == 0x0a)) {
                func[i] = 0;
                break;
            } else {
                func[i] = line[i];
            }

            i++;

        }

        pclose(p);

    }

    return len;
}


void addSymbol( unsigned int address )
{
    int index;

    for (index = 0 ; index < MAX_FUNCTIONS ; index++) {
        if (functions[index].address == address) {
            return;
        }

        if (functions[index].address == 0) {
            break;
        }

    }

    if (index < MAX_FUNCTIONS) {
        functions[index].address = address;
        translateFunctionFromSymbol( address, functions[index].funcName );

    } else {
        assert( 0 );
    }

    return;
}


void addCallTrace( unsigned int address )
{
    if (stackNumElems()) {
        calls[lookupSymbol(stackTop())][lookupSymbol(address)]++;
    }

    return;
}

void reverse(char *str, int len) {
    char tmp;
    for(int i=0; i<len/2; i++){
        tmp = str[i];
        str[i] = str[len-1-i];
        str[len-1-i] = tmp;
    }
}

void setFloatStr(double vlu, char *str) {
    const char chr1000group = ',';
    int len = sprintf(str, "%'.3f", vlu);
    //printf("\n\n0: %s\n", str);

    if(len<=7){
        //0.123, 10.123, 100.123
        return;
    }

    if(strchr(str, chr1000group)){
        //格式化后的字符串中已经含有逗号,不用再额外处理,直接退出.
        //有无千分符,取决于Linux的locale中的LC_NUMERIC设置.默认的
        return;
    }

    //12345678.901
    reverse(str, len);
    //109.87654321
    //printf("\n\n1: %s\n", str);

    int cnt = 0;
    int idx = 4;
    while(len-idx >= 3){
        memcpy(&str[idx+4], &str[idx+3], len-idx);
        //109.876,54321
        //109.876,543,21
        str[idx+3] = chr1000group;
        idx += 4;
        //printf("\n\n2.: %s, len=%d, idx=%d\n", str, len, idx);
        cnt++;
    }

    //printf("\n\n3: %s\n", str);
    //109.876,543,21
    reverse(str, len+cnt);
    //12,345,678.901
    //printf("\n\n4: %s\n", str);
}

void emitSymbols( __u64 nano_hex_max )
{
    int from, to;
    FILE *fp;

    fp = fopen("graph.dot", "w");
    if (fp == NULL) {
        printf("Couldn't open graph.dot\n");
        exit(0);
    }

    char *strColors[] = {
        "color=yellow fillcolor=red fontcolor=white",
        "fillcolor=orange",
        "fillcolor=yellow",
        "",
    };
    int indexOfColors = 3;
    double tmUS = 0;
    char tmUSstr[16] = {0};

    fprintf(fp, "digraph %s {\n\n", imageName );
    fprintf( fp, "    rankdir=LR;\n");
    fprintf( fp, "    edge [ fontcolor=\"#606060\" color=\"#808080\" fontsize=10]\n");
    fprintf( fp, "    node [shape=rectangle style=filled fillcolor=\"#f8f8f0\" color=\"#606080\" fontcolor=black]\n");

    /* Identify node shapes */
    for (from = 0 ; from < MAX_FUNCTIONS ; from++) {
        if (functions[from].address == 0) {
            break;
        }

        if(from==0 && strcmp(functions[from].funcName, "main")==0){
            //main函数
            printf(">>0x%llx - 0x%llx = %lld\n", nano_hex_max, functions[from].ns, nano_hex_max - functions[from].ns);
            functions[from].ns = nano_hex_max - functions[from].ns;
            __u64 tmSec = functions[from].ns>>32; //高8字节是秒，低8字节是纳秒；
            int tmH = tmSec / 3600;
            int tmM = (tmSec % 3600)/60;
            int tmS = tmSec % 60;

            printf(">>%llu; tmH:%d; tmM:%d; tmS:%d\n", tmSec, tmH, tmM, tmS);

            fprintf( fp, "  \"%s\" [ shape=ellipse label=\"%s\\n%02d:%02d:%02d\" style=\"bold,filled\" fontsize=24 fillcolor=\"#e0e0f0\"]\n", 
                    functions[from].funcName, 
                    functions[from].funcName, tmH, tmM, tmS );
            continue;
        }

        for (to = 0 ; to < MAX_FUNCTIONS ; to++) {
            if (functions[to].address == 0) {
                break;
            }

            if (calls[from][to]) {
                totals[from]++;
            }
        }

        //这里显示是是平均耗时ns；若需要最大值，则改为nsMax即可；
        tmUS = functions[from].ns/1000.0;
        if(tmUS>=100000.000)
            indexOfColors = 0;
        else if(tmUS>=1000.000)
            indexOfColors = 1;
        else if(tmUS >= 10.000)
            indexOfColors = 2;
        else
            indexOfColors = 3;

        //tmUS=12345678.901f;
        setFloatStr(tmUS, tmUSstr);

        if (totals[from]) {
            fprintf( fp, "  \"%s\" [label=\"%s\\n%s us\" %s]\n", 
                    functions[from].funcName, 
                    functions[from].funcName, tmUSstr, strColors[indexOfColors] );
        } else {
            fprintf( fp, " \"%s\" [style=\"filled,rounded\" label=\"%s\\n%s us\" %s]\n", 
                    functions[from].funcName, 
                    functions[from].funcName, tmUSstr, strColors[indexOfColors] );
        }

    }

    /* Emit call graph */
    for (from = 0 ; from < MAX_FUNCTIONS ; from++) {
        if (functions[from].address == 0) {
            break;
        }

        for (to = 0 ; to < MAX_FUNCTIONS ; to++) {
            if (calls[from][to]) {
                fprintf( fp, "  \"%s\" -> \"%s\" [label=\"%d calls\" fontsize=\"10\"]\n", 
                        functions[from].funcName, functions[to].funcName,
                        calls[from][to] );
            }

            if (functions[to].address == 0) break;

        }

    }

    fprintf( fp, "\n}\n" );
    fclose(fp);

    return;
}

void setFuncTime( int addr, __u64 ns ){
    int index = lookupSymbol(addr);
    if(functions[index].ns ==0){
        functions[index].ns = ns;
        functions[index].nsMax = ns;
        functions[index].nsMin = ns;
    }
    else {
        //记下最大值和最小值
        if(ns > functions[index].nsMax) { functions[index].nsMax = ns; }
        if(ns < functions[index].nsMin) { functions[index].nsMin = ns; }
        //平均值(加权平均，最后的调用权最重)
        functions[index].ns += ns;
        functions[index].ns >>= 1;
    }

    printf("0x%x:%12llu,%12llu\t%12llu,%12llu\t%-8s\n", addr, ns, functions[index].ns, functions[index].nsMin, functions[index].nsMax, functions[index].funcName);
}

