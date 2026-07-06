#ifndef _MAIN_PROCESSING_H_
#define _MAIN_PROCESSING_H_

#include <time.h>

#include "stack_array.h"
#include "stack_list.h"
#include "char_stack.h"

#define COMPARE_NUMS 1000

#define ERROR_INPUT 1
#define ERROR_STACK_EMPTY 4
#define ERROR_STACK_FULL 5
#define ERROR_STACK_MALLOC 6
#define ERROR_EMPTY_STRING 7

void clear_input_buffer(void);
void show_menu(void);
void print_error(int rc);
void process_array_stack(void);  
void process_list_stack(void);  
void proccess_is_palindrom(void);
void compare_performance(void);

#endif 
