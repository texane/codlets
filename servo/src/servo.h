#ifndef SERVO_H_INCLUDED
# define SERVO_H_INCLUDED

void servo_setup(void);
void servo_move(unsigned char index, unsigned int value);

#if CONFIG_DEBUG
void servo_print(void);
#endif /* CONFIG_DEBUG */

#endif /* ! SERVO_H_INCLUDED */
