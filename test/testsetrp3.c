#include <unity.h>
#include "setrp3.h"

extern unsigned char product;
extern unsigned int credit;

void setUp(void)
{
	return;
}
void tearDown(void)
{
	return;
}

void test_product_state_is_correct(void)
{
	resetInitialState();
	TEST_ASSERT_EQUAL_INT(0, product);
	changeSelectedProduct(-1);
	TEST_ASSERT_EQUAL_INT(2, product);
	changeSelectedProduct(-1);
	TEST_ASSERT_EQUAL_INT(1, product);
	changeSelectedProduct(-1);
	TEST_ASSERT_EQUAL_INT(0, product);
	changeSelectedProduct(1);
	TEST_ASSERT_EQUAL_INT(1, product);
	changeSelectedProduct(1);
	TEST_ASSERT_EQUAL_INT(2, product);
	changeSelectedProduct(1);
	TEST_ASSERT_EQUAL_INT(0, product);
}

void test_insert_coin(void)
{
	resetInitialState();
	TEST_ASSERT_EQUAL_INT(0, credit);
	insertCoin(0);
	TEST_ASSERT_EQUAL_INT(10, credit);
	insertCoin(1);
	TEST_ASSERT_EQUAL_INT(30, credit);
	insertCoin(2);
	TEST_ASSERT_EQUAL_INT(80, credit);
	insertCoin(3);
	TEST_ASSERT_EQUAL_INT(180, credit);
	int res = insertCoin(4);
	TEST_ASSERT_EQUAL_INT(res, -1);
	res = insertCoin(-1);
	TEST_ASSERT_EQUAL_INT(res, -1);
}

void test_loop(void)
{
	resetInitialState();
	eventLoop(0, 0);
	eventLoop(COININSERT, 0);
	eventLoop(0, 0);
	TEST_ASSERT_EQUAL_INT(10, credit);
	eventLoop(COININSERT, 1);
	eventLoop(0, 0);
	TEST_ASSERT_EQUAL_INT(30, credit);
	eventLoop(CHANGEPRODUCT, 1);
	eventLoop(0, 0);
	TEST_ASSERT_EQUAL_INT(1, product);
	eventLoop(RETURNCREDIT, 0);
	eventLoop(0, 0);
	TEST_ASSERT_EQUAL_INT(0, credit);
	eventLoop(COININSERT, 3);
	eventLoop(0, 0);
	TEST_ASSERT_EQUAL_INT(100, credit);
	eventLoop(SELECTPRODUCT, 0);
	eventLoop(0, 0);
	TEST_ASSERT_EQUAL_INT(0, credit);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_product_state_is_correct);
	RUN_TEST(test_insert_coin);
	RUN_TEST(test_loop);
	return UNITY_END();
}
