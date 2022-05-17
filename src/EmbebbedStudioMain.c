/**
 * @file EmbebbedStudioMain.c
 * @author João Ferreira (joaoprcf@ua.pt), Daniel Oliveira (danielsoliveira@ua.pt), João Carvalho (jpmffc@ua.pt)
 * @brief SETR lab 3 project
 * @version 0.1
 * @date 2022-05-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/util.h>
#include <sys/printk.h>
#include <inttypes.h>

#define NUM_PRODUCTS 3 ///< Number of available products
#define NUM_COINS 4    ///< Number of coins accepted

#define IDLE 0          ///< Idle state identifier
#define COININSERT 1    ///< Coin insertion state identifier
#define CHANGEPRODUCT 2 ///< Change product state identifier
#define SELECTPRODUCT 3 ///< Select product state identifier
#define RETURNCREDIT 4  ///< Return credit state identifier

/* SOLUTION CODE */

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

/**
 * @brief Insert coin logic, the credit is increased according to the value of the inserted coin.
 *
 * @param[in] index Coin index to be inserted
 * @return int
 */
int insertCoin(int index)
{
  if (index < 0 || index >= NUM_COINS)
  {
    return -1;
  }
  credit += COIN_VALUE[index];
  return credit;
}

/**
 * @brief Buy selected product to be dispensed if enough credit is available.
 *
 * @return int
 */
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

/**
 * @brief Return credit to user, setting the current device value to 0.
 *
 * @return int
 */
int returnCredit(void)
{
  unsigned int cpyCredit = credit;
  credit = 0;
  return cpyCredit;
}

/**
 * @brief Change the current selected product
 *
 * @param[in] direction Either -1 or 1 to select the previous or the next product respectively.
 * @return int
 */
int changeSelectedProduct(int direction)
{
  if (direction != -1 && direction != 1)
    return product;
  product = (product + NUM_PRODUCTS + direction) % NUM_PRODUCTS;
  return product;
}

/**
 * @brief Reset initial state.
 *
 */
void resetInitialState(void)
{
  credit = 0.0;
  product = 0;
  return;
}

/**
 * @brief Manage events and define corresponding actions to the state machine.
 *
 * @param[in] event Event triggered
 * @param[in] index Event external data
 */
void eventLoop(char event, int index)
{
  int cred;
  switch (state)
  {
  case IDLE:
    state = event;
    eventdata = index;
    break;
  case COININSERT:
    insertCoin(eventdata);
    printk("Current credit: %d.%02d EUR\n", credit / 100, credit % 100);
    state = IDLE;
    break;
  case CHANGEPRODUCT:
    changeSelectedProduct(eventdata ? 1 : -1);
    printk("Current selected product: %s, price %d.%02d EUR\n", productName[product], PRICE[product] / 100, PRICE[product] % 100);
    state = IDLE;
    break;
  case SELECTPRODUCT:
    if (select())
    {
      printk("Product %s dispensed, remaining credit: %d.%02d EUR\n", productName[product], credit / 100, credit % 100);
    }
    else
    {
      printk("Not enough credit, Product %s costs %d.%02d EUR, credit is %d.%02d EUR\n", productName[product], PRICE[product] / 100, PRICE[product] % 100, credit / 100, credit % 100);
    }
    state = IDLE;
    break;
  case RETURNCREDIT:
    cred = returnCredit();
    printk("%d.%02d EUR return\n", cred / 100, cred % 100);
    state = IDLE;
    break;
  default:
    break;
  }
}

/* END SOLUTION CODE */

#define SLEEP_TIME_MS 1                              ///< Sleep time between loop iterations
#define GPIO0_NID DT_NODELABEL(gpio0)                ///< Alias for the node label
#define SW0_NODE DT_ALIAS(sw0)                       ///< Alias for internal button 0
#define SW1_NODE DT_ALIAS(sw1)                       ///< Alias for internal button 1
#define SW2_NODE DT_ALIAS(sw2)                       ///< Alias for internal button 2
#define SW3_NODE DT_ALIAS(sw3)                       ///< Alias for internal button 3
#define SW4_NODE 0x1c                                ///< Alias for external button 0
#define SW5_NODE 0x1d                                ///< Alias for external button 1
#define SW6_NODE 0x1e                                ///< Alias for external button 2
#define SW7_NODE 0x1f                                ///< Alias for external button 3
#define INT_BUTTON_NUM 4                             ///< Internal button number
#define EXT_BUTTON_NUM 4                             ///< External button number
#define BUTTON_NUM (INT_BUTTON_NUM + EXT_BUTTON_NUM) ///< Total number of buttons

static const struct gpio_dt_spec buttons[] = {
    GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0}),
    GPIO_DT_SPEC_GET_OR(SW1_NODE, gpios, {0}),
    GPIO_DT_SPEC_GET_OR(SW2_NODE, gpios, {0}),
    GPIO_DT_SPEC_GET_OR(SW3_NODE, gpios, {0}),
};

static char SWNODE[] = {
    SW4_NODE,
    SW5_NODE,
    SW6_NODE,
    SW7_NODE};

static struct gpio_callback button_cb_data[BUTTON_NUM];

typedef struct debouncer
{
  unsigned int dcIndex;
  char state;
};

static struct debouncer buttondb[BUTTON_NUM];

const struct device *gpio0_dev[EXT_BUTTON_NUM];

/**
 * @brief Main
 *
 * @return int
 */
int main(void)
{

  for (int btidx = 0; btidx < INT_BUTTON_NUM; btidx++)
  {
    if (btidx < 4 && !device_is_ready(buttons[btidx].port))
    {
      printk("Error: buttons[%d] device %s is not ready\n",
             btidx, buttons[btidx].port->name);
      return 1;
    }

    int ret;
    ret = gpio_pin_configure_dt(&buttons[btidx], GPIO_INPUT);

    if (ret != 0)
    {
      printk("Error %d: failed to configure %s pin %d\n",
             ret, buttons[btidx].port->name, buttons[btidx].pin);
      return 1;
    }

    buttondb[btidx].dcIndex = 0;
    buttondb[btidx].state = 0;
  }

  for (int btidx = 0; btidx < EXT_BUTTON_NUM; btidx++)
  {
    int ret;
    gpio0_dev[btidx] = device_get_binding(DT_LABEL(GPIO0_NID));
    ret = gpio_pin_configure(gpio0_dev[btidx], SWNODE[btidx], GPIO_INPUT | GPIO_PULL_UP);
    if (ret != 0)
    {
      printk("Error %d: failed to configure external button at index %d\n",
             ret, btidx);
      return 1;
    }
    printk("ret: %d\n", ret);

    buttondb[INT_BUTTON_NUM + btidx].dcIndex = 0;
    buttondb[INT_BUTTON_NUM + btidx].state = 0;
  }

  printf("Current credit: %d.%02d EUR\n", credit / 100, credit % 100);
  while (1)
  {

    int event = IDLE;
    int evdata = 0;
    for (int btidx = 0; btidx < BUTTON_NUM; btidx++)
    {

      int val;
      if (btidx < INT_BUTTON_NUM)
        val = gpio_pin_get_dt(&buttons[btidx]);
      else
      {
        val = 1 - gpio_pin_get(gpio0_dev[btidx - INT_BUTTON_NUM], SWNODE[btidx - INT_BUTTON_NUM]);
      }
      if (val > 0)
      {
        buttondb[btidx].dcIndex += 1;
        if (buttondb[btidx].dcIndex == 5)
        {
          buttondb[btidx].state = 1;
          switch (btidx)
          {
          case 4:
            event = CHANGEPRODUCT;
            evdata = 0;
            break;
          case 5:
            event = CHANGEPRODUCT;
            evdata = 1;
            break;
          case 6:
            event = SELECTPRODUCT;
            evdata = 0;
            break;
          case 7:
            event = RETURNCREDIT;
            evdata = 0;
            break;
          default:
            event = COININSERT;
            evdata = btidx;
          }
        }
      }
      else
      {
        buttondb[btidx].dcIndex = 0;
        buttondb[btidx].state = 0;
      }
    }
    eventLoop(event, evdata);
    k_msleep(SLEEP_TIME_MS);
  }
  return 0;
}
