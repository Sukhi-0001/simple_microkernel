#ifndef ISR80H_H
#define ISR80H_H

enum SystemCommands
{
    SYSTEM_COMMAND0_SUM,
    SYSTEM_COMMAND1_,
    SYSTEM_COMMAND2_,
    SYSTEM_COMMAND3_,
    SYSTEM_COMMAND4_SYS_WRITE
};

void isr80h_register_commands();

#endif