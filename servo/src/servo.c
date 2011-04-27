#define CONFIG_TIMER_COUNT 8
#define CONFIG_INITIAL_TIMER 2000

typedef unsigned char index_t;
typedef unsigned int value_t;

#define INVALID_INDEX ((index_t)-1)
#define TAIL_INDEX CONFIG_TIMER_COUNT
#define HEAD_INDEX (next[TAIL_INDEX])


/* circular linked list */

static value_t timer[CONFIG_TIMER_COUNT + 1];
static index_t prev[CONFIG_TIMER_COUNT + 1];
static index_t next[CONFIG_TIMER_COUNT + 1];

static value_t current_timer;
static index_t current_index;


/* interrupt handler */
void timer_interrupt(void)
{
  /* when the current timer reaches 0, assign latd and reload */
  if ((current_timer--) == 0)
  {
#if 0 /* TODO */
    /* if this is the last event, reset latdbits */
    if (current_index == TAIL_INDEX)
      LATD = 0xffff;
    else
      LATD &= ~(1 << current_index);
#endif /* TODO */

    current_index = next[current_index];
    current_timer = timer[current_index];
  }
}


void servo_setup(void)
{
  index_t i;

  /* reset nodes */
  for (i = 0; i < (CONFIG_TIMER_COUNT + 1); ++i)
  {
    next[i] = i;
    prev[i] = i;
  }

  /* init the list */
  timer[TAIL_INDEX] = CONFIG_INITIAL_TIMER;
  prev[TAIL_INDEX] = TAIL_INDEX;
  next[TAIL_INDEX] = TAIL_INDEX;

  /* bootstrap timers */
  current_index = TAIL_INDEX;
  current_timer = 0;
}


#if 0 /* TODO */

static unsigned int was_enabled;

static inline void disable_timer_interrupt(void)
{ was_enabled = ADIE; }

static inline void restore_timer_interrupt(void)
{ if (was_enabled) ADIE = 1; }

#endif /* TODO */


/* set a servo move value */
void servo_move(unsigned int index, unsigned int value)
{
  /* index: the servo index, < CONFIG_TIMER_COUNT
     esp. index must be != CONFIG_TIMER_COUNT
     value: the servo value, <= 2000
   */

  value_t sum;
  index_t prev_index;
  index_t next_index;

#if 0 /* TODO */
  /* disable interrupt temporarly */
  disable_timer_interrupt();
#endif /* TODO */

  /* if linked, update nexts and unlink */
  next_index = next[index];
  if (next_index != index)
  {
    prev_index = prev[index];

    /* next[index] valid since cannot be TAIL_INDEX */
    timer[next_index] += timer[index];

    next[prev_index] = next_index;
    prev[next_index] = prev_index;

    prev[index] = index;
    next[index] = index;
  }

#if 0 /* TODO */
  /* reenable interrupt */
  restore_timer_interrupt();
#endif /* TODO */

  /* find the index where to insert _before_ such that:
     next[TAIL_INDEX] <-> .. <-> prev_index <-> i <-> .. <-> TAIL_INDEX
     sum = integrates timer values over [0 - prev_index]
   */
  sum = 0;
  prev_index = TAIL_INDEX;
  next_index = HEAD_INDEX;
  while (1)
  {
    const value_t tmp = sum + timer[next_index];
    if (tmp >= value) break ;
    prev_index = next_index;
    next_index = next[next_index];
    sum = tmp;
  }

  /* timer values */
  timer[index] = value - sum;

  /* update next timer */
  timer[next_index] += sum - value;

#if 0 /* TODO */
  /* disable interrupt temporarly */
  disable_timer_interrupt();
#endif /* TODO */

  /* insert into list */
  next[prev_index] = index;
  prev[next_index] = index;
  prev[index] = prev_index;
  next[index] = next_index;

#if 0 /* TODO */
  restore_timer_interrupt(was_enabled);
#endif /* TODO */
}


#if CONFIG_DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void servo_print(void)
{
  index_t i;
  value_t sum = 0;

  printf("---\n");

  i = HEAD_INDEX;
  while (1)
  {
    sum += timer[i];
    printf(" %4u", sum);
    if (i == TAIL_INDEX) break ;
    i = next[i];
  }
  printf("\n");

  i = HEAD_INDEX;
  while (1)
  {
    printf(" %4u", timer[i]);
    if (i == TAIL_INDEX) break ;
    i = next[i];
  }
  printf("\n");
}

int main(int ac, char** av)
{
  servo_setup();

  servo_move(0, 1000);
  servo_print();

  servo_move(1, 500);
  servo_print();

  servo_move(4, 1500);
  servo_print();

  servo_move(3, 100);
  servo_print();

  /* bug */
  servo_move(0, 50);
  servo_print();
  /* bug */

  return 0;
}

#endif /* CONFIG_DEBUG */
