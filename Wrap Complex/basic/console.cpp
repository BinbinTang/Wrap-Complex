#include "console.h"
#include <windows.h>
#include <cstdlib>
#include <cstdio>

void print(char* str)
{
	unsigned long i;
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),str,strlen(str)+1,&i,NULL);
}

void print(double d)
{
	unsigned long i;
	char str[255];
	//sprintf(str,"%6.2f",d);
	sprintf_s(str,255,"%6.2",d);
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),str,strlen(str)+1,&i,NULL);
}

void print(int d)
{
	unsigned long i;
	char str[255];
	//sprintf(str,"%d",d);
	sprintf_s(str,255,"%d",d);
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),str,strlen(str)+1,&i,NULL);
}