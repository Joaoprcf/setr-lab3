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

/**
 * @brief Insert coin logic, the credit is increased according to the value of the inserted coin.
 *
 * @param[in] index Coin index to be inserted
 * @return int
 */
int insertCoin(int index);

/**
 * @brief Buy selected product to be dispensed if enough credit is available.
 *
 * @return int
 */
int select(void);

/**
 * @brief Return credit to user, setting the current device value to 0.
 *
 * @return int
 */
int returnCredit(void);

/**
 * @brief Change the current selected product
 *
 * @param[in] direction Either -1 or 1 to select the previous or the next product respectively.
 * @return int
 */
int changeSelectedProduct(int);

/**
 * @brief Reset initial state.
 *
 */
void resetInitialState(void);

/**
 * @brief Manage events and define corresponding actions to the state machine.
 *
 * @param[in] event Event triggered
 * @param[in] index Event external data
 */
void eventLoop(char event, int index);

#endif
