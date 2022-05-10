/* ***************************************************** */
/* SETR 21/22, Paulo Pedreiras                           */
/* Base code for Unit Testing                            */
/*   A few tests to the cmdProcessor                     */
/*      just to illustrate how it works                  */
/*   Shoud be improved (e.g. test more cases)            */
/*                                                       */
/* Compile with: gcc cmdproc.c main.c -o main            */
/*                                                       */
/* ***************************************************** */
#include <stdio.h>
#include "setrp3.h"

int main(void)
{
	printf("OK\n");
	while (1)
	{
		resetInitialState();
		eventLoop(0, 0);
		eventLoop(COININSERT, 0);
		eventLoop(0, 0);
		eventLoop(COININSERT, 1);
		eventLoop(0, 0);
		eventLoop(CHANGEPRODUCT, 1);
		eventLoop(0, 0);
		eventLoop(RETURNCREDIT, 0);
		eventLoop(0, 0);
		eventLoop(COININSERT, 3);
		eventLoop(0, 0);
		eventLoop(SELECTPRODUCT, 0);
		eventLoop(0, 0);
	}
}
