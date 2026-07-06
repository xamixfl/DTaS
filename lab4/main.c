#include "main_processing.h"

int main(void)
{
    int choice;
    do
    {
        show_menu();
        if (scanf("%d", &choice) != 1)
        {
            print_error(ERROR_INPUT);
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice)
        {
            case 1:
                process_array_stack();
                break;
            case 2:
                process_list_stack();
                break;
            case 3:
                proccess_is_palindrom();
                break;
            case 4:
                compare_performance();
                break;
            case 0:
                printf("Завершение работы.\n");
                break;
            default:
                printf("Нет такого варианта ответа.\n");
        }

    } while (choice != 0);

    return 0;
}
