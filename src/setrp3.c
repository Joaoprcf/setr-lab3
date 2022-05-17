#include <stdio.h>
#include "setrp3.h"

/* CONSTANTS */
unsigned int PRICE[NUM_PRODUCTS] = {150, 100, 50};
unsigned int COIN_VALUE[NUM_COINS] = {10, 20, 50, 100};

#define MAX_STRING_SIZE 15

char productName[][MAX_STRING_SIZE] =
	{"Beer",
	 "Tuna sandwich",
	 "Coffee"};

/* State parameters */
/* Note that in a real application these vars would be extern */
unsigned int credit = 0;
unsigned char product = 0;
unsigned char state = IDLE;
unsigned char eventdata = 0;

int insertCoin(int index)
{
	if (index < 0 || index >= NUM_COINS)
	{
		return -1;
	}
	credit += COIN_VALUE[index];
	return credit;
}

int select(void)
{
	if (PRICE[product] <= credit)
	{
		credit -= PRICE[product];
		return 1;
	}
	else
	{
		return 0;
	}

	return credit;
}

int returnCredit(void)
{
	unsigned int cpyCredit = credit;
	credit = 0;
	return cpyCredit;
}

int changeSelectedProduct(int direction)
{
	if (direction != -1 && direction != 1)
		return product;
	product = (product + NUM_PRODUCTS + direction) % NUM_PRODUCTS;
	return product;
}

void resetInitialState(void)
{
	credit = 0.0;
	product = 0;
	return;
}

void eventLoop(char event, int index)
{
	int cred;
	switch (state)
	{
	case IDLE:
		printf("Current credit: %d.%02d EUR\n", credit / 100, credit % 100);
		state = event;
		eventdata = index;
		break;
	case COININSERT:
		insertCoin(eventdata);
		printf("Current credit: %d.%02d EUR\n", credit / 100, credit % 100);
		state = IDLE;
		break;
	case CHANGEPRODUCT:
		changeSelectedProduct(eventdata ? 1 : -1);
		printf("Current selected product: %s, price %d.%02d EUR\n", productName[product], PRICE[product] / 100, PRICE[product] % 100);
		state = IDLE;
		break;
	case SELECTPRODUCT:
		if (select())
		{
			printf("Product %s dispensed, remaining credit: %d.%02d EUR\n", productName[product], credit / 100, credit % 100);
		}
		else
		{
			printf("Not enough credit, Product %s costs %d.%02d EUR, credit is %d.%02d EUR\n", productName[product], PRICE[product] / 100, PRICE[product] % 100, credit / 100, credit % 100);
		}
		state = IDLE;
		break;
	case RETURNCREDIT:
		cred = returnCredit();
		printf("%d.%02d EUR return\n", cred / 100, cred % 100);
		state = IDLE;
		break;
	default:
		break;
	}
}