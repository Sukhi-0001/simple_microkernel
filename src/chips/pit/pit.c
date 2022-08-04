#include "pit.h"
#include "../../io/io.h"


void pit_init(){
    outb(PIT_CMD,PIT_DEFAULT_SETTING);
    pit_set_count(PIT_DEFAULT_INTERVAL);
}

void pit_set_count(unsigned count) {
	// Disable interrupts
 
	// Set low byte
	outb(0x40,count&0xFF);		// Low byte
	outb(0x40,(count&0xFF00)>>8);	// High byte
	return;
}