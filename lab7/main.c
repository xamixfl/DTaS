#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>

#include "bst.h"
#include "avl.h"
#include "hashtable_open.h"
#include "hashtable_closed.h"
#include "graphviz.h"

#define MAX_LEVELS 100
#define NUM_COMPARE_OPS 1000
#define MAX_STRING_LENGTH 2000
#define DEFAULT_HT_SIZE 13

#define OK 0
#define ERR_MEM_ALLOC 1
#define ERR_EMPTY_TREE 2
#define ERR_INVALID_INPUT 3
#define ERR_ELEM_NO_IN 4

double get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double) tv.tv_sec * 1000.0 + (double) tv.tv_usec / 1000.0;
}

void print_error(int rc)
{
    switch (rc)
    {
        case OK:
            break;
        case ERR_MEM_ALLOC:
            printf("Ошибка: Не удалось выделить память.\n");
            break;
        case ERR_EMPTY_TREE:
            printf("Ошибка: Дерево пусто.\n");
            break;
        case ERR_INVALID_INPUT:
            printf("Ошибка: Неверный ввод данных.\n");
            break;
        case ERR_ELEM_NO_IN:
            break;
        case HT_OPEN_CHAR_ERR_MEM:
            printf("Ошибка: Не удалось выделить память для хеш-таблицы.\n");
            break;
        case HT_OPEN_CHAR_ERR_FULL:
            printf("Ошибка: Хеш-таблица переполнена!\n");
            break;
        case HT_CLOSED_CHAR_ERR_MEM:
            printf("Ошибка: Не удалось выделить память для хеш-таблицы.\n");
            break;
        default:
            printf("Неизвестная ошибка: %d\n", rc);
            break;
    }
}

void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void show_menu()
{
    printf("\nМеню:\n");
    printf("1. Ввести строку и построить все структуры\n");
    printf("2. Вывести все структуры на экран\n");
    printf("3. Показать количество узлов на уровнях деревьев\n");
    printf("4. Добавить букву (все структуры)\n");
    printf("5. Удалить букву (все структуры)\n");
    printf("6. Поиск буквы и замеры\n");
    printf("7. Сравнение производительности\n");
    printf("8. Тест хеш-таблиц при коллизиях\n");
    printf("9. Реструктуризовать хеш-таблицы вручную\n");
    printf("10. Экспорт деревьев в изображения\n");
    printf("0. Выход\n");
    printf("Введите номер действия: ");
}

int handle_load_data(
    bst_node_char_t **bst_root,
    avl_node_char_t **avl_root,
    hashtable_open_char_t **ht_open,
    hashtable_closed_char_t **ht_closed,
    char *string_buffer)
{
    printf("Введите строку (только буквы латиницы, до %d символов): ", MAX_STRING_LENGTH - 1);
    
    // СНАЧАЛА пробуем прочитать строку
    if (fgets(string_buffer, MAX_STRING_LENGTH, stdin) == NULL)
        return ERR_INVALID_INPUT;

    size_t len = strlen(string_buffer);
    if (len > 0 && string_buffer[len - 1] == '\n')
        string_buffer[len - 1] = '\0';

    // Очистка старых данных
    bst_char_free(*bst_root);
    *bst_root = NULL;
    avl_char_free(*avl_root);
    *avl_root = NULL;
    
    if (*ht_open)
        hashtable_open_char_free(*ht_open);
    if (*ht_closed)
        hashtable_closed_char_free(*ht_closed);

    // Запрос размера хеш-таблиц
    int table_size;
    printf("Введите размер хеш-таблиц (рекомендуется простое число): ");
    
    char size_input[20];
    if (fgets(size_input, sizeof(size_input), stdin) == NULL)
    {
        table_size = DEFAULT_HT_SIZE;
        printf("Используется размер по умолчанию: %d\n", table_size);
    }
    else
    {
        if (sscanf(size_input, "%d", &table_size) != 1 || table_size <= 0)
        {
            table_size = DEFAULT_HT_SIZE;
            printf("Используется размер по умолчанию: %d\n", table_size);
        }
    }

    // Создание новых структур
    *ht_open = hashtable_open_char_create(table_size);
    *ht_closed = hashtable_closed_char_create(table_size);
    
    if (!*ht_open || !*ht_closed)
    {
        hashtable_open_char_free(*ht_open);
        hashtable_closed_char_free(*ht_closed);
        *ht_open = NULL;
        *ht_closed = NULL;
        return ERR_MEM_ALLOC;
    }

    // Построение всех структур из строки
    int inserted_count = 0;
    int insert_error = OK;
    
    for (int i = 0; string_buffer[i] != '\0'; i++)
    {
        if (isalpha(string_buffer[i]))
        {
            char c = string_buffer[i];
            
            // Пробуем вставить во все структуры
            *bst_root = bst_char_insert(*bst_root, c);
            *avl_root = avl_char_insert(*avl_root, c);
            
            // ПРОВЕРЯЕМ ОШИБКИ ОТ ХЕШ-ТАБЛИЦ!
            int rc_open = hashtable_open_char_insert(*ht_open, c);
            int rc_closed = hashtable_closed_char_insert(*ht_closed, c);
            
            if (rc_open != OK || rc_closed != OK)
            {
                insert_error = rc_open;  // Запоминаем ошибку
                break;  // Выходим из цикла
            }
            
            inserted_count++;
        }
    }
    
    // ЕСЛИ БЫЛА ОШИБКА ПЕРЕПОЛНЕНИЯ
    if (insert_error == HT_OPEN_CHAR_ERR_FULL)
    {
        printf("\n   ОШИБКА: Хеш-таблица переполнена!\n");
        printf("   Размер таблицы: %d, Попытка вставить: >%d элементов\n", 
               (*ht_open)->size, (*ht_open)->size);
        printf("   Нужно увеличить размер таблицы или уменьшить строку.\n");
        
        // Очищаем ВСЁ, что успели создать
        bst_char_free(*bst_root); *bst_root = NULL;
        avl_char_free(*avl_root); *avl_root = NULL;
        hashtable_open_char_free(*ht_open); *ht_open = NULL;
        hashtable_closed_char_free(*ht_closed); *ht_closed = NULL;
        
        string_buffer[0] = '\0';  // Очищаем строку
        
        return HT_OPEN_CHAR_ERR_FULL;
    }

    printf("✓ Все структуры построены из строки\n");
    printf("  Строка: \"%s\" (длина: %zu)\n", string_buffer, len);
    printf("  Букв в строке: %d\n", inserted_count);
    
    if (*bst_root)
        printf("  BST высота: %d\n", bst_char_get_height(*bst_root));
    else
        printf("  BST: пусто\n");
        
    if (*avl_root)
        printf("  AVL высота: %d\n", avl_char_get_height(*avl_root));
    else
        printf("  AVL: пусто\n");
        
    printf("  Хеш-таблицы: размер %d\n", table_size);
    printf("  Открытая HT: %d элементов\n", (*ht_open)->count);
    printf("  Закрытая HT: %d элементов\n", (*ht_closed)->count);

    return OK;
}

int handle_print_all(
    bst_node_char_t *bst_root,
    avl_node_char_t *avl_root,
    hashtable_open_char_t *ht_open,
    hashtable_closed_char_t *ht_closed)
{
    if (!bst_root && !avl_root && (!ht_open || ht_open->count == 0) && (!ht_closed || ht_closed->count == 0))
        return ERR_EMPTY_TREE;

    printf("\nBST (обычное дерево поиска):\n");
    if (bst_root)
        bst_char_print(bst_root);
    else
        printf("(пусто)\n");

    printf("\nAVL (сбалансированное дерево):\n");
    if (avl_root)
        avl_char_print(avl_root);
    else
        printf("(пусто)\n");

    printf("\nХеш-таблица (открытая адресация):\n");
    if (ht_open && ht_open->count > 0)
    {
        printf("Размер: %d, Элементов: %d\n", ht_open->size, ht_open->count);
        hashtable_open_char_print(ht_open);
        printf("Макс. коллизии: %d\n", hashtable_open_char_max_collisions(ht_open));
        printf("Макс. сравнения: %d\n", hashtable_open_char_max_comparisons(ht_open));
    }
    else
        printf("(пусто)\n");

    printf("\nХеш-таблица (закрытая адресация):\n");
    if (ht_closed && ht_closed->count > 0)
    {
        printf("Размер: %d, Элементов: %d\n", ht_closed->size, ht_closed->count);
        hashtable_closed_char_print(ht_closed);
        printf("Макс. коллизии: %d\n", hashtable_closed_char_max_collisions(ht_closed));
        printf("Макс. сравнения: %d\n", hashtable_closed_char_max_comparisons(ht_closed));
    }
    else
        printf("(пусто)\n");

    return OK;
}

int handle_count_levels(
    bst_node_char_t *bst_root,
    avl_node_char_t *avl_root)
{
    if (bst_root == NULL && avl_root == NULL)
        return ERR_EMPTY_TREE;

    int bst_counts[MAX_LEVELS] = {0};
    int bst_max_depth = 0;
    bst_char_count_levels(bst_root, bst_counts, 0, &bst_max_depth, MAX_LEVELS);

    int avl_counts[MAX_LEVELS] = {0};
    int avl_max_depth = 0;
    avl_char_count_levels(avl_root, avl_counts, 0, &avl_max_depth, MAX_LEVELS);

    printf("\nСтатистика по уровням (BST):\n");
    for (int i = 0; i <= bst_max_depth && i < MAX_LEVELS; i++)
        printf("Уровень %d: %d узлов\n", i, bst_counts[i]);

    printf("\nСтатистика по уровням (AVL):\n");
    for (int i = 0; i <= avl_max_depth && i < MAX_LEVELS; i++)
        printf("Уровень %d: %d узлов\n", i, avl_counts[i]);

    return OK;
}

int handle_add_single(
    bst_node_char_t **bst_root,
    avl_node_char_t **avl_root,
    hashtable_open_char_t *ht_open,
    hashtable_closed_char_t *ht_closed)
{
    char c;
    printf("Введите букву для добавления: ");
    if (scanf(" %c", &c) != 1)
    {
        clear_input_buffer();
        return ERR_INVALID_INPUT;
    }
    clear_input_buffer();

    if (!isalpha(c))
    {
        printf("Можно добавлять только буквы латиницы.\n");
        return ERR_INVALID_INPUT;
    }
    
    // 1. СНАЧАЛА пытаемся добавить в хеш-таблицы
    if (ht_open)
    {
        int rc = hashtable_open_char_insert(ht_open, c);
        if (rc == HT_OPEN_CHAR_ERR_FULL)
        {
            printf("   Ошибка: Открытая хеш-таблица переполнена!\n");
            printf("   Размер: %d, Элементов: %d\n", ht_open->size, ht_open->count);
            printf("   Используйте реструктуризацию (пункт 9).\n");
            return HT_OPEN_CHAR_ERR_FULL;
        }
        else if (rc != OK)
        {
            printf("   Ошибка при добавлении в открытую хеш-таблицу: %d\n", rc);
            return rc;
        }
    }
    
    if (ht_closed)
    {
        int rc = hashtable_closed_char_insert(ht_closed, c);
        if (rc != OK)  // Проверяем любую ошибку
        {
            // Если открытая таблица уже добавилась, у нас проблема
            // Но без функции удаления мы не можем откатить
            printf("   Ошибка: Закрытая хеш-таблица переполнена!\n");
            printf("   Размер: %d, Элементов: %d\n", ht_closed->size, ht_closed->count);
            printf("   Внимание: данные могут быть неконсистентными!\n");
            return rc;  // Просто возвращаем ошибку
        }
    }
    
    // 2. ТОЛЬКО ЕСЛИ хеш-таблицы успешны, добавляем в деревья
    *bst_root = bst_char_insert(*bst_root, c);
    *avl_root = avl_char_insert(*avl_root, c);
    
    printf("Буква '%c' добавлена во все структуры.\n", c);
    return OK;
}


int handle_delete_single(
    bst_node_char_t **bst_root,
    avl_node_char_t **avl_root,
    hashtable_open_char_t *ht_open,
    hashtable_closed_char_t *ht_closed)
{
    char c;
    printf("Введите букву для удаления: ");
    if (scanf(" %c", &c) != 1)
    {
        clear_input_buffer();
        return ERR_INVALID_INPUT;
    }
    clear_input_buffer();

    if (!isalpha(c))
    {
        printf("Можно удалять только буквы латиницы.\n");
        return ERR_INVALID_INPUT;
    }

    int success_bst = 0, success_avl = 0, success_open = 0, success_closed = 0;

    if (*bst_root)
        *bst_root = bst_char_delete(*bst_root, c, &success_bst);
    
    if (*avl_root)
        *avl_root = avl_char_delete(*avl_root, c, &success_avl);
    
    if (ht_open)
        success_open = hashtable_open_char_delete(ht_open, c);
    
    if (ht_closed)
        success_closed = hashtable_closed_char_delete(ht_closed, c);

    if (success_bst || success_avl || success_open || success_closed)
        printf("Буква '%c' удалена.\n", c);
    else
    {
        printf("Буква '%c' не найдена.\n", c);
        return ERR_ELEM_NO_IN;
    }

    return OK;
}

int handle_search_compare(
    bst_node_char_t *bst_root,
    avl_node_char_t *avl_root,
    hashtable_open_char_t *ht_open,
    hashtable_closed_char_t *ht_closed,
    const char *current_string)
{
    char c;
    printf("Введите букву для поиска: ");
    if (scanf(" %c", &c) != 1)
        return ERR_INVALID_INPUT;
    clear_input_buffer();

    if (!isalpha(c))
    {
        printf("Можно искать только буквы латиницы.\n");
        return ERR_INVALID_INPUT;
    }

    int cmp_bst = 0, cmp_avl = 0, cmp_open = 0, cmp_closed = 0;
    
    int cmp_string = 0;
    double start = get_time_ms();
    int found_string = 0;
    
    // Линейный поиск в строке
    for (int i = 0; current_string[i] != '\0'; i++)
    {
        cmp_string++;
        if (current_string[i] == c)
        {
            found_string = 1;
            break;
        }
    }
    double time_string = get_time_ms() - start;

    start = get_time_ms();
    bst_node_char_t *res_bst = bst_char_search(bst_root, c, &cmp_bst);
    double time_bst = get_time_ms() - start;

    start = get_time_ms();
    avl_node_char_t *res_avl = avl_char_search(avl_root, c, &cmp_avl);
    double time_avl = get_time_ms() - start;

    start = get_time_ms();
    int found_open = ht_open ? hashtable_open_char_search(ht_open, c, &cmp_open) : 0;
    double time_open = get_time_ms() - start;

    start = get_time_ms();
    int found_closed = ht_closed ? hashtable_closed_char_search(ht_closed, c, &cmp_closed) : 0;
    double time_closed = get_time_ms() - start;

    printf("\nРЕЗУЛЬТАТЫ ПОИСКА '%c':\n", c);
    printf("BST: %s", res_bst ? "Найдено" : "Не найдено");
    printf(", Сравнений: %d, Время: %.3f мс\n", cmp_bst, time_bst);
    
    printf("AVL: %s", res_avl ? "Найдено" : "Не найдено");
    printf(", Сравнений: %d, Время: %.3f мс\n", cmp_avl, time_avl);
    
    printf("Открытая HT: %s, Сравнений: %d, Время: %.3f мс\n", 
           found_open ? "Найдено" : "Не найдено", cmp_open, time_open);
    
    printf("Закрытая HT: %s, Сравнений: %d, Время: %.3f мс\n", 
           found_closed ? "Найдено" : "Не найдено", cmp_closed, time_closed);
           
    printf("Строка: %s, Сравнений: %d, Время: %.3f мс\n", 
           found_string ? "Найдено" : "Не найдено", cmp_string, time_string);

    long long mem_bst = bst_char_memory_usage(bst_root);
    long long mem_avl = avl_char_memory_usage(avl_root);
    long long mem_open = ht_open ? hashtable_open_char_memory_usage(ht_open) : 0;
    long long mem_closed = ht_closed ? hashtable_closed_char_memory_usage(ht_closed) : 0;

    printf("\nПамять:\n");
    printf("  BST: %lld байт\n", mem_bst);
    printf("  AVL: %lld байт\n", mem_avl);
    printf("  Открытая HT: %lld байт\n", mem_open);
    printf("  Закрытая HT: %lld байт\n", mem_closed);
    printf("  Строка: %zu байт\n", strlen(current_string));

    return OK;
}

int handle_rehash(
    hashtable_open_char_t **ht_open,
    hashtable_closed_char_t **ht_closed,
    const char *source_string)
{
    if (!*ht_open || !*ht_closed || !source_string)
    {
        printf("Хеш-таблицы или строка не созданы.\n");
        return ERR_EMPTY_TREE;
    }

    int new_size;
    printf("Введите новый размер хеш-таблиц (текущий: %d): ", (*ht_open)->size);
    if (scanf("%d", &new_size) != 1 || new_size <= 0)
    {
        clear_input_buffer();
        return ERR_INVALID_INPUT;
    }
    clear_input_buffer();

    if (new_size <= (*ht_open)->size)
    {
        printf("Новый размер должен быть больше текущего.\n");
        return ERR_INVALID_INPUT;
    }

    printf("Перестраиваем открытую хеш-таблицу...\n");
    
    // 1. Уничтожаем старую таблицу
    hashtable_open_char_free(*ht_open);
    
    // 2. Создаём новую таблицу с новым размером
    *ht_open = hashtable_open_char_create(new_size);
    
    // 3. ЗАНОВО строим таблицу из исходной строки
    for (int i = 0; source_string[i] != '\0'; i++)
    {
        if (isalpha(source_string[i]))
        {
            char c = tolower(source_string[i]);
            hashtable_open_char_insert(*ht_open, c);
        }
    }
    
    printf("Перестраиваем закрытую хеш-таблицу...\n");
    
    // Аналогично для закрытой
    hashtable_closed_char_free(*ht_closed);
    *ht_closed = hashtable_closed_char_create(new_size);
    
    for (int i = 0; source_string[i] != '\0'; i++)
    {
        if (isalpha(source_string[i]))
        {
            char c = tolower(source_string[i]);
            hashtable_closed_char_insert(*ht_closed, c);
        }
    }

    printf("✓ Таблицы перестроены");
    return OK;
}

int perform_test(
    const char *description,
    const char *test_string)
{
    printf("\n--- %s ---\n", description);
    printf("Строка: \"%s\" (длина: %zu)\n", test_string, strlen(test_string));

    bst_node_char_t *bst = NULL;
    avl_node_char_t *avl = NULL;
    hashtable_open_char_t *ht_open = hashtable_open_char_create(DEFAULT_HT_SIZE);
    hashtable_closed_char_t *ht_closed = hashtable_closed_char_create(DEFAULT_HT_SIZE);

    if (!ht_open || !ht_closed)
    {
        hashtable_open_char_free(ht_open);
        hashtable_closed_char_free(ht_closed);
        return ERR_MEM_ALLOC;
    }

    // Построение структур
    double insert_time_bst = 0, insert_time_avl = 0, insert_time_open = 0, insert_time_closed = 0;
    double insert_time_string = 0; // Время обработки строки

    int letter_count = 0;
    
    // Измеряем время обработки строки (подсчёт букв)
    double start_string = get_time_ms();
    int counts[256] = {0};
    for (int i = 0; test_string[i] != '\0'; i++)
    {
        if (isalpha(test_string[i]))
        {
            char c = test_string[i];
            counts[(unsigned char)c]++;
        }
    }
    insert_time_string = get_time_ms() - start_string;

    for (int i = 0; test_string[i] != '\0'; i++)
    {
        if (isalpha(test_string[i]))
        {
            char c = test_string[i];
            letter_count++;

            double start = get_time_ms();
            bst = bst_char_insert(bst, c);
            insert_time_bst += get_time_ms() - start;

            start = get_time_ms();
            avl = avl_char_insert(avl, c);
            insert_time_avl += get_time_ms() - start;

            start = get_time_ms();
            hashtable_open_char_insert(ht_open, c);
            insert_time_open += get_time_ms() - start;

            start = get_time_ms();
            hashtable_closed_char_insert(ht_closed, c);
            insert_time_closed += get_time_ms() - start;
        }
    }

    if (letter_count == 0)
    {
        printf("Строка не содержит букв.\n");
        bst_char_free(bst);
        avl_char_free(avl);
        hashtable_open_char_free(ht_open);
        hashtable_closed_char_free(ht_closed);
        return ERR_INVALID_INPUT;
    }

    // Метрики памяти и высоты
    long long mem_bst = bst_char_memory_usage(bst);
    long long mem_avl = avl_char_memory_usage(avl);
    long long mem_open = hashtable_open_char_memory_usage(ht_open);
    long long mem_closed = hashtable_closed_char_memory_usage(ht_closed);
    
    int height_bst = bst_char_get_height(bst);
    int height_avl = avl_char_get_height(avl);

    // Подготовка уникальных букв для тестирования поиска
    char unique_letters[256];
    int unique_count = 0;
    memset(counts, 0, sizeof(counts));
    
    for (int i = 0; test_string[i] != '\0'; i++)
    {
        if (isalpha(test_string[i]))
        {
            char c = test_string[i];
            if (counts[(unsigned char)c] == 0)
                unique_letters[unique_count++] = c;
            counts[(unsigned char)c]++;
        }
    }

    // Тестирование поиска в строках
    double search_time_string = 0;
    double avg_cmp_string = 0;
    
    // Поиск в строке (линейный поиск)
    for (int i = 0; i < NUM_COMPARE_OPS; i++)
    {
        char c = unique_letters[rand() % unique_count];
        int cmp = 0;
        
        double start = get_time_ms();
        // Линейный поиск в строке
        for (int j = 0; test_string[j] != '\0'; j++)
        {
            cmp++;
            if (test_string[j] == c)
                break;
        }
        search_time_string += get_time_ms() - start;
        avg_cmp_string += cmp;
    }

    // Тестирование поиска в структурах
    double search_time_bst = 0, search_time_avl = 0, search_time_open = 0, search_time_closed = 0;
    double avg_cmp_bst = 0, avg_cmp_avl = 0, avg_cmp_open = 0, avg_cmp_closed = 0;
    
    int max_coll_open = hashtable_open_char_max_collisions(ht_open);
    int max_coll_closed = hashtable_closed_char_max_collisions(ht_closed);

    for (int i = 0; i < NUM_COMPARE_OPS; i++)
    {
        char c = unique_letters[rand() % unique_count];
        int cmp = 0;

        double start = get_time_ms();
        bst_char_search(bst, c, &cmp);
        search_time_bst += get_time_ms() - start;
        avg_cmp_bst += cmp;

        cmp = 0;
        start = get_time_ms();
        avl_char_search(avl, c, &cmp);
        search_time_avl += get_time_ms() - start;
        avg_cmp_avl += cmp;

        cmp = 0;
        start = get_time_ms();
        hashtable_open_char_search(ht_open, c, &cmp);
        search_time_open += get_time_ms() - start;
        avg_cmp_open += cmp;

        cmp = 0;
        start = get_time_ms();
        hashtable_closed_char_search(ht_closed, c, &cmp);
        search_time_closed += get_time_ms() - start;
        avg_cmp_closed += cmp;
    }

    avg_cmp_bst /= (double)NUM_COMPARE_OPS;
    avg_cmp_avl /= (double)NUM_COMPARE_OPS;
    avg_cmp_open /= (double)NUM_COMPARE_OPS;
    avg_cmp_closed /= (double)NUM_COMPARE_OPS;
    avg_cmp_string /= (double)NUM_COMPARE_OPS;

    // Тестирование удаления
    double delete_time_bst = 0, delete_time_avl = 0, delete_time_open = 0, delete_time_closed = 0;
    double delete_time_string = 0;

    // Перемешиваем буквы для удаления
    char *delete_letters = malloc(unique_count);
    memcpy(delete_letters, unique_letters, unique_count);
    for (int i = unique_count - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        char tmp = delete_letters[i];
        delete_letters[i] = delete_letters[j];
        delete_letters[j] = tmp;
    }

    // Удаление из строки (удаление всех вхождений буквы)
    for (int i = 0; i < unique_count; i++)
    {
        char c = delete_letters[i];
        int success = 0;

        // Удаление из строки
        double start = get_time_ms();
        char temp_string[MAX_STRING_LENGTH];
        strcpy(temp_string, test_string);
        int write_idx = 0;
        for (int j = 0; temp_string[j] != '\0'; j++)
        {
            if (temp_string[j] != c)
                temp_string[write_idx++] = temp_string[j];
        }
        temp_string[write_idx] = '\0';
        delete_time_string += get_time_ms() - start;

        // Удаление из структур
        start = get_time_ms();
        bst = bst_char_delete(bst, c, &success);
        delete_time_bst += get_time_ms() - start;

        success = 0;
        start = get_time_ms();
        avl = avl_char_delete(avl, c, &success);
        delete_time_avl += get_time_ms() - start;

        start = get_time_ms();
        hashtable_open_char_delete(ht_open, c);
        delete_time_open += get_time_ms() - start;

        start = get_time_ms();
        hashtable_closed_char_delete(ht_closed, c);
        delete_time_closed += get_time_ms() - start;
    }

    free(delete_letters);
    
    
    // Вывод результатов в виде таблицы
    printf("------------------------------------------------------------------------------------------------\n");
    printf("|                         | BST        | AVL      | Открытая HT   | Закрытая HT   | Строка     |\n");
    printf("------------------------------------------------------------------------------------------------\n");
    printf("| Ср. время вставки (мс)  | %-10.6f | %-8.6f | %-13.6f | %-13.6f | %-10.6f |\n",
           insert_time_bst / letter_count, insert_time_avl / letter_count,
           insert_time_open / letter_count, insert_time_closed / letter_count,
           insert_time_string / letter_count);
           printf("------------------------------------------------------------------------------------------------\n");
    printf("| Ср. время поиска (мс)   | %-10.6f | %-8.6f | %-13.6f | %-13.6f | %-10.6f |\n",
           search_time_bst / NUM_COMPARE_OPS, search_time_avl / NUM_COMPARE_OPS,
           search_time_open / NUM_COMPARE_OPS, search_time_closed / NUM_COMPARE_OPS,
           search_time_string / NUM_COMPARE_OPS);
           printf("------------------------------------------------------------------------------------------------\n");
    printf("|Ср. к-во срав. при поиске| %-10.6f | %-8.6f | %-13.6f | %-13.6f | %-10.6f |\n",
           avg_cmp_bst, avg_cmp_avl, avg_cmp_open, avg_cmp_closed, avg_cmp_string);
    printf("------------------------------------------------------------------------------------------------\n");
    printf("| Ср. время удаления (мс) | %-10.6f | %-8.6f | %-13.6f | %-13.6f | %-10.6f |\n",
           delete_time_bst / unique_count, delete_time_avl / unique_count,
           delete_time_open / unique_count, delete_time_closed / unique_count,
           delete_time_string / unique_count);
    printf("------------------------------------------------------------------------------------------------\n");
    printf("| Макс. коллизий          | -          | -        | %-13d | %-13d | -          |\n",
           max_coll_open, max_coll_closed);
    printf("------------------------------------------------------------------------------------------------\n");
    printf("| Память (байт)           | %-10lld | %-8lld | %-13lld | %-13lld | %-10lld |\n",
           mem_bst, mem_avl, mem_open, mem_closed, (long long)strlen(test_string));
    printf("------------------------------------------------------------------------------------------------\n");
    printf("| Высота дерева           | %-10d | %-8d | -             | -             | -          |\n",
           height_bst, height_avl);
    printf("------------------------------------------------------------------------------------------------\n");

    bst_char_free(bst);
    avl_char_free(avl);
    hashtable_open_char_free(ht_open);
    hashtable_closed_char_free(ht_closed);

    return OK;
}

int handle_performance_test(void)
{
    printf("\nСравнение производительности:\n");

    // Случайная строка
    char random_string[MAX_STRING_LENGTH];
    int random_len = 500;
    
    printf("Генерация случайной строки из %d букв...\n", random_len);
    srand(time(NULL));
    for (int i = 0; i < random_len; i++)
        random_string[i] = 'a' + (rand() % 26);
    random_string[random_len] = '\0';

    // Отсортированная строка (вырожденное дерево)
    char sorted_string[MAX_STRING_LENGTH];
    int sorted_len = 500;
    
    printf("Генерация отсортированной строки из %d букв...\n", sorted_len);
    for (int i = 0; i < sorted_len; i++)
        sorted_string[i] = 'a' + (i % 26);
    sorted_string[sorted_len] = '\0';

    perform_test("Случайная строка (Сбалансированное)", random_string);
    perform_test("Отсортированная строка (Вырожденное)", sorted_string);

    return OK;
}

int handle_collision_test(void)
{
    printf("\nТест коллизий:\n");

    int sizes[] = {7, 11, 17};
    for (int s = 0; s < 3; s++)
    {
        int size = sizes[s];
        printf("\nРазмер таблицы: %d\n", size);

        hashtable_open_char_t *open = hashtable_open_char_create(size);
        hashtable_closed_char_t *closed = hashtable_closed_char_create(size);

        // Создаём коллизии: вставляем буквы с одинаковым хешем
        for (int i = 0; i < size; i++)
        {
            // Используем буквы, которые будут иметь одинаковый хеш при размере таблицы
            char key = 'a' + (i * size) % 26;
            hashtable_open_char_insert(open, key);
            hashtable_closed_char_insert(closed, key);
        }

        int max_cmp_o = hashtable_open_char_max_comparisons(open);
        int max_cmp_c = hashtable_closed_char_max_comparisons(closed);

        printf("До реструктуризации:\n");
        printf("  Открытая: сравнений = %d, макс.коллизий = %d\n", 
               max_cmp_o, hashtable_open_char_max_collisions(open));
        printf("  Закрытая: сравнений = %d, макс.коллизий = %d\n", 
               max_cmp_c, hashtable_closed_char_max_collisions(closed));

        // Реструктуризация
        hashtable_open_char_rehash(open, (int)(size * 1.3));
        hashtable_closed_char_rehash(closed, (int)(size * 1.3));

        max_cmp_o = hashtable_open_char_max_comparisons(open);
        max_cmp_c = hashtable_closed_char_max_comparisons(closed);

        printf("После реструктуризации (размер %d):\n", open->size);
        printf("  Открытая: сравнений = %d, макс.коллизий = %d\n", 
               max_cmp_o, hashtable_open_char_max_collisions(open));
        printf("  Закрытая: сравнений = %d, макс.коллизий = %d\n", 
               max_cmp_c, hashtable_closed_char_max_collisions(closed));

        hashtable_open_char_free(open);
        hashtable_closed_char_free(closed);
    }
    return OK;
}

int handle_graphics_export(
    bst_node_char_t *bst_root,
    avl_node_char_t *avl_root)
{
    if (!bst_root && !avl_root)
    {
        printf("Деревья пусты. Сначала введите строку.\n");
        return ERR_EMPTY_TREE;
    }

    printf("\nЭкспорт деревьев в PNG...\n");
    
    if (bst_root)
    {
        bst_char_export_to_dot_and_png(bst_root, "bst_tree");
    }
    
    if (avl_root)
    {
        avl_char_export_to_dot_and_png(avl_root, "avl_tree");
    }

    return OK;
}

int main(void)
{
    bst_node_char_t *bst_root = NULL;
    avl_node_char_t *avl_root = NULL;
    hashtable_open_char_t *ht_open = NULL;
    hashtable_closed_char_t *ht_closed = NULL;
    
    char current_string[MAX_STRING_LENGTH] = "";
    int choice;
    
    srand(time(NULL));
    do
    {
        show_menu(current_string);
        if (scanf("%d", &choice) != 1)
        {
            print_error(ERR_INVALID_INPUT);
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        int rc = OK;
        switch (choice)
        {
            case 1:
                rc = handle_load_data(&bst_root, &avl_root, &ht_open, &ht_closed, current_string);
                break;
            case 2:
                rc = handle_print_all(bst_root, avl_root, ht_open, ht_closed);
                break;
            case 3:
                rc = handle_count_levels(bst_root, avl_root);
                break;
            case 4:
                rc = handle_add_single(&bst_root, &avl_root, ht_open, ht_closed);
                break;
            case 5:
                rc = handle_delete_single(&bst_root, &avl_root, ht_open, ht_closed);
                break;
            case 6:
                rc = handle_search_compare(bst_root, avl_root, ht_open, ht_closed, current_string);
                break;
            case 7:
                rc = handle_performance_test();
                break;
            case 8:
                rc = handle_collision_test();
                break;
            case 9:
                rc = handle_rehash(&ht_open, &ht_closed, current_string);
                break;
            case 10:
                rc = handle_graphics_export(bst_root, avl_root);
                break;
            case 0:
                printf("Завершение работы.\n");
                break;
            default:
                print_error(ERR_INVALID_INPUT);
        }
        if (rc)
            print_error(rc);
    } while (choice != 0);

    // Очистка памяти
    bst_char_free(bst_root);
    avl_char_free(avl_root);
    if (ht_open)
        hashtable_open_char_free(ht_open);
    if (ht_closed)
        hashtable_closed_char_free(ht_closed);

    return OK;
}
