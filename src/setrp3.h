#ifndef __SETR_P3_H_
#define __SETR_P3_H_

/* constants */
#define NUM_PRODUCTS 3
#define NUM_COINS 4

#define IDLE 0
#define COININSERT 1
#define CHANGEPRODUCT 2
#define SELECTPRODUCT 3
#define RETURNCREDIT 4

/* Function prototypes */
int insertCoin(int index);
int select(void);
int changeSelectedProduct(int);
void resetInitialState(void);
void eventLoop(char event, int index);

#endif
