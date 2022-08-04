#include "isr80h.h"
#include "../idt/idt.h"
#include "misc.h"

void isr80h_register_commands()
{
    isr80h_register_command(SYSTEM_COMMAND0_SUM, isr80h_command0_sum);
    isr80h_register_command(SYSTEM_COMMAND4_SYS_WRITE,isr80h_command4_sys_write);
}