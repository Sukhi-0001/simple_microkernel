#ifndef PIT_H
#define PIT_H

#define PIT_COUNTER0 0x40
#define PIT_CMD      0x43

#define PIT_FREQUENCY 1193182
#define PIT_DEFAULT_INTERVAL PIT_FREQUENCY/100
#define PIT_DEFAULT_SETTING 0b00110000

void pit_init();
void pit_set_count(unsigned count);
#endif
