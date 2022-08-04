#ifndef KERNEL_H
#define KERNEL_H
#include <stdint.h>
#define VGA_WIDTH 80
#define VGA_HEIGHT 20
#define TOTAL_GDT_SEGMENTS 6

#define KERNEL_CODE_SELECTOR 0X08
#define KERNEL_DATA_SELECTOR 0X10


void kernel_main();
void kernel_registers();
void kernel_page();
void terminal_init();
uint16_t make_char(char a,char colour);
void terminal_put_char(int x, int y, char c, char colour);
void kernel_print_uint(uint32_t to_print);
void panic(char *str);
void kernel_print_string(char *str,uint32_t len);
void kernel_print_char_hex(uint8_t hex);
void kernel_print(char *str);
void kernel_print_str_hex(void *buffer,uint32_t length);

#endif