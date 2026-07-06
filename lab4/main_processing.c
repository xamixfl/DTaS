#include "main_processing.h"

void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void show_menu(void)
{
    printf("\n--- Меню ---\n");
    printf("1. Работа со стеком как статическим массивом\n");
    printf("2. Работа со стеком как с односвязным списком\n");
    printf("3. Определить, является ли строка палиндромом\n");
    printf("4. Сравнить производительность и память\n");
    printf("0. Выход\n");
    printf("Введите номер действия: ");
}

void print_error(int rc)
{
    switch (rc)
    {
        case 0:
            break;
        case ERROR_INPUT:
            printf("Ошибка: некорректный ввод данных.\n");
            break;
        case ERROR_STACK_EMPTY:
            printf("Ошибка: Стек пуст.\n");
            break;
        case ERROR_STACK_FULL:
            printf("Ошибка: Стек полон.\n");
            break;
        case ERROR_STACK_MALLOC:
            printf("Ошибка: Не удалось выделить память для стека.\n");
            break;
        case ERROR_EMPTY_STRING:
            printf("Ошибка: Введена пустая строка.\n");
            break;
        default:
            printf("Неизвестная ошибка с кодом: %d\n", rc);
            break;
    }
}

void process_array_stack(void)                                                                                      // Работа со стеком как с массивом
{
    stack_arr_t stack;
    stack_array_init(&stack);
    int choice, rc;
    char val;
    do
    {
        printf("\n--- Стек как массив ---\n");
        printf("1. Добавить элемент\n");
        printf("2. Удалить элемент\n");
        printf("3. Вывести состояние стека\n");
        printf("0. Назад в главное меню\n");
        printf("Введите номер действия: ");

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
                printf("Введите целое число для добавления: ");
                if (scanf("%c", &val) != 1)
                {
                    print_error(ERROR_INPUT);
                    clear_input_buffer();
                    break;
                }
                clear_input_buffer();
                rc = stack_array_push(&stack, val);
                if (rc) 
                    print_error(rc);
                else 
                    printf("Элемент %c добавлен.\n", val);
                break;
            case 2:
                rc = stack_array_pop(&stack, &val);
                if (rc) 
                    print_error(rc);
                else 
                    printf("Элемент %c удален.\n", val);
                break;
            case 3:
                stack_array_print(&stack);
                break;
            case 0:
                break;
            default:
                print_error(ERROR_INPUT);
        }

    } while (choice != 0);
}

void process_list_stack(void)                                                                                         // Работа со стеком как со списком
{
    ListStack_t stack;
    FreeList_t freelist;
    int choice, rc;
    char val;

    list_stack_init(&stack);
    free_list_init(&freelist);

    do
    {
        printf("\n--- Стек как список ---\n");
        printf("1. Добавить элемент\n");
        printf("2. Удалить элемент\n");
        printf("3. Вывести состояние стека\n");
        printf("4. Просмотр свободных адресов стека\n");
        printf("0. Назад в главное меню\n");
        printf("Введите номер действия: ");

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
                printf("Введите целое число для добавления: ");
                if (scanf("%c", &val) != 1)
                {
                    print_error(ERROR_INPUT);
                    clear_input_buffer();
                    break;
                }
                clear_input_buffer();
                rc = list_stack_push(&stack, val);
                if (rc) 
                    print_error(rc);
                else 
                    printf("Элемент %d добавлен.\n", val);
                break;
            case 2:
                rc = list_stack_pop(&stack, &val, &freelist);
                if (rc) 
                    print_error(rc);
                else 
                    printf("Элемент %d удален.\n", val);
                break;
            case 3:
                list_stack_print(&stack);
                break;
            case 4:
                free_list_print(&freelist);
            case 0:
                break;
            default:
                print_error(ERROR_INPUT);
        }
        
    } while (choice != 0);
    list_stack_free(&stack);
    free_list_free(&freelist);
}

void proccess_is_palindrom(void) 
{
    char str[1000];
    printf("\n--- Определение на палиндром при помощи стека ---\n");
    printf("Введите строку: ");
    
    if (fgets(str, sizeof(str), stdin) == NULL) 
    {
        print_error(ERROR_INPUT);
        return;
    }

    size_t len = strlen(str);
    if (len == 1 && str[0] == '\n') 
    {
        print_error(ERROR_EMPTY_STRING);
        return;
    }
    if (len > 0 && str[len-1] == '\n') 
    {
        str[len-1] = '\0';
        len--;
    }

    CharStack_t s;
    char_stack_init(&s);
    bool is_pal = true;

    for (size_t i = 0; i < len; i++) 
    {
        if (!char_stack_push(&s, str[i])) 
        {
            print_error(ERROR_STACK_FULL);
            char_stack_free(&s);
            return;
        }
    }

    for (size_t i = 0; i < len; i++) 
    {
        char popped;
        if (!char_stack_pop(&s, &popped)) 
        {
            print_error(ERROR_STACK_EMPTY);
            is_pal = false;
            break;
        }
        if (popped != str[i]) 
        {
            is_pal = false;
            break;
        }
    }

    if (!char_stack_is_empty(&s)) 
        is_pal = false;

    char_stack_free(&s);

    printf("%s\n", is_pal ? 
        "Строка является палиндромом." : 
        "Строка не является палиндромом.");
}

void compare_performance(void) 
{
    printf("\n==== Сравнение эффективности (%d операций) ====\n", COMPARE_NUMS);

    clock_t start, end;
    double t_arr_push = 0, t_arr_pop = 0;
    double t_list_push = 0, t_list_pop = 0;
    double t_pal_arr = 0, t_pal_list = 0;
    char temp;

    const char* test_strings[] = {
        "racecar", "abba", "level", "rotor", "12321",
        "A man a plan a canal Panama", "Was it a car or a cat I saw?"
    };
    const int num_tests = 7;
    const int repeat = COMPARE_NUMS / 10; 

    // Массив
    stack_arr_t arr;
    stack_array_init(&arr);
    start = clock();
    for (int i = 0; i < COMPARE_NUMS; i++)
        stack_array_push(&arr, (char)(i % 128));
    end = clock();
    t_arr_push = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < COMPARE_NUMS; i++)
        stack_array_pop(&arr, &temp);
    end = clock();
    t_arr_pop = (double)(end - start) / CLOCKS_PER_SEC;

    // Список
    ListStack_t list;
    list_stack_init(&list);

    start = clock();
    for (int i = 0; i < COMPARE_NUMS; i++)
        list_stack_push(&list, (char)(i % 128));
    end = clock();
    t_list_push = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < COMPARE_NUMS; i++) 
    {
        char temp;
        list_stack_pop(&list, &temp, NULL);
    }
    end = clock();
    t_list_pop = (double)(end - start) / CLOCKS_PER_SEC;

    // Палиндром на массиве
    start = clock();
    for (int r = 0; r < repeat; r++) 
    {
        for (int i = 0; i < num_tests; i++) 
        {
            char str[1000];
            strcpy(str, test_strings[i]);
            size_t len = strlen(str);
            if (len > 0 && str[len-1] == ' ') str[len-1] = '\0'; 

            stack_arr_t s;
            stack_array_init(&s);
            for (size_t j = 0; j < len; j++)
                stack_array_push(&s, str[j]);
            for (size_t j = 0; j < len; j++) 
            {
                char c;
                stack_array_pop(&s, &c);
            }
        }
    }
    end = clock();
    t_pal_arr = (double)(end - start) / CLOCKS_PER_SEC;

    // Палиндром на списке
    start = clock();
    for (int r = 0; r < repeat; r++) 
    {
        for (int i = 0; i < num_tests; i++) 
        {
            char str[1000];
            strcpy(str, test_strings[i]);
            size_t len = strlen(str);

            ListStack_t s;
            list_stack_init(&s);

            for (size_t j = 0; j < len; j++)
                list_stack_push(&s, str[j]);
            for (size_t j = 0; j < len; j++) 
            {
                char c;
                list_stack_pop(&s, &c, NULL); 
            }
            list_stack_free(&s); 
        }
    }
    end = clock();
    t_pal_list = (double)(end - start) / CLOCKS_PER_SEC;

    list_stack_free(&list);

    // Память 
    size_t mem_arr = sizeof(stack_arr_t);
    size_t mem_list = COMPARE_NUMS * sizeof(Node_t);

    // ТАБЛИЦА
    printf("\n| Реализация          | push, с      | pop, с      | палиндром, с | память, байт |\n");
    printf("|---------------------|--------------|-------------|--------------|--------------|\n");
    printf("| Массив              | %.10f | %.10f | %.10f |    %zu    |\n",
           t_arr_push, t_arr_pop, t_pal_arr, mem_arr);
    printf("| Список              | %.10f | %.10f | %.10f |    %zu    |\n",
           t_list_push, t_list_pop, t_pal_list, mem_list);
    printf("----------------------------------------------------------------------------------\n");

    printf("\n--- Анализ точки равновесия по памяти ---\n");

    size_t mem_array_static  = sizeof(stack_arr_t);           // 1004 байт
    size_t mem_list_base     = sizeof(ListStack_t);           // 16 байт
    size_t mem_list_per_node = sizeof(Node_t);                // 16 байт

    printf("Память стека-массива (статично, %d эл.): %zu байт\n", 
        ARR_STACK_MAX_SIZE, mem_array_static);
    printf("Память стека-списка (базовая, 0 эл.): %zu байт\n", mem_list_base);
    printf("Память на 1 узел списка: %zu байт\n", mem_list_per_node);

    // Защита
    if (mem_list_per_node == 0 || ARR_STACK_MAX_SIZE == 0) {
        printf("Ошибка: деление на ноль.\n");
        return;
    }

    // Точка равновесия: N, при котором список ≤ массива
    size_t max_N = (mem_array_static - mem_list_base) / mem_list_per_node;
    size_t tipping_N = max_N + 1;

    // Проценты
    double max_percent = (double)max_N / ARR_STACK_MAX_SIZE * 100.0;
    double tip_percent = (double)tipping_N / ARR_STACK_MAX_SIZE * 100.0;

    // Память при N
    size_t mem_at_max = mem_list_base + max_N * mem_list_per_node;
    size_t mem_at_tip = mem_list_base + tipping_N * mem_list_per_node;

    printf("\n");
    printf("Стек-список выигрывает по памяти при N ≤ %zu (%.2f%% от размера массива):\n", 
        max_N, max_percent);
    printf("  → Память: %zu байт (≤ %zu)\n", mem_at_max, mem_array_static);

    printf("Стек-массив выигрывает при N ≥ %zu (%.2f%% от размера массива):\n", 
        tipping_N, tip_percent);
    printf("  → Память списка: %zu байт (> %zu)\n", mem_at_tip, mem_array_static);

    printf("\nТочка равновесия: при %zu элементах память равна или меньше у списка.\n", max_N);
    printf("При %zu элементах — массив начинает выигрывать.\n", tipping_N);
}

