#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "tree.h"

tree_node_t *root = NULL;
char last_input_string[1024] = "";

void clear_input_buffer(void) 
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int handle_build_tree(void) 
{
    printf("Введите строку из букв: ");
    if (!fgets(last_input_string, sizeof(last_input_string), stdin))
        return ERR_INVALID_INPUT;

    last_input_string[strcspn(last_input_string, "\n")] = '\0';

    tree_free(root);
    root = NULL;
    int cnt = 0;
    for (int i = 0; last_input_string[i]; i++) {
        if (isalpha((unsigned char)last_input_string[i]))
        {
            root = tree_insert(root, last_input_string[i]);
            cnt++;
        }
    }
    if (!cnt) 
    {
        print_red("Нет букв в строке!\n");
        return ERR_EMPTY_TREE;
    }
    print_green("Дерево построено (все буквы вставлены, включая повторы)\n");
    return 0;
}

int handle_print_tree_colored(void) 
{
    if (!root) 
        return ERR_EMPTY_TREE;
    int freq[256] = {0};

    count_frequency(root, freq);
    print_yellow("\n=== Двоичное дерево поиска ===\n");
    print_red("Красный — буква встречается более одного раза\n");
    print_green("Зелёный — уникальная буква\n");
    tree_print_colored(root, 0, freq);
    printf("\n");
    return 0;
}

int handle_remove_duplicates(void) 
{
    if (!root) 
        return ERR_EMPTY_TREE;

    print_yellow("Удаляем все повторяющиеся буквы...\n");
    root = remove_duplicates_from_tree(root);
    print_green("Готово! Остались только уникальные буквы.\n");
    return 0;
}

int handle_postorder(void)
{
    if (!root) 
    {
        print_red("Дерево пусто — все буквы были удалены.\n");
        return ERR_EMPTY_TREE;
    }
    printf("Постфиксный обход: ");
    tree_traverse_postorder(root);
    printf("\n");
    return 0;
}

int handle_compare_performance(void) 
{
    if (!last_input_string[0]) 
    {
        print_red("Сначала введите строку (пункт 1)\n");
        return ERR_INVALID_INPUT;
    }
    clock_t s, e;
    double t_tree = 0, t_str = 0;

    tree_node_t *tmp = NULL;
    for (const char *p = last_input_string; *p; p++)
    {
        if (isalpha((unsigned char)*p)) 
            tmp = tree_insert(tmp, *p);
    }
        
    s = clock();
    tmp = remove_duplicates_from_tree(tmp);
    e = clock();
    t_tree = (e - s) / (double)CLOCKS_PER_SEC;
    tree_free(tmp);

    s = clock();
    char *res = remove_duplicates_from_string(last_input_string);
    e = clock();
    t_str = (e - s) / (double)CLOCKS_PER_SEC;
    free(res);

    print_yellow("\n=== Сравнение времени ===\n");
    printf("Дерево: %.6f сек\n", t_tree);
    printf("Строка: %.6f сек\n", t_str);
    if (t_tree > t_str + 1e-9)
        printf(COLOR_RED "Строка быстрее в %.1fx\n" COLOR_RESET, t_tree/t_str);
    else
        printf(COLOR_GREEN "Дерево быстрее\n" COLOR_RESET);
    return 0;
}

int handle_preorder(void) 
{
    if (!root) { print_red("Дерево пусто!\n"); return ERR_EMPTY_TREE; }
    printf("Префиксный обход: ");
    tree_traverse_preorder(root);
    printf("\n");
    return 0;
}

int handle_inorder(void) 
{
    if (!root) { print_red("Дерево пусто!\n"); return ERR_EMPTY_TREE; }
    printf("Инфиксный обход: ");
    tree_traverse_inorder(root);
    printf("\n");
    return 0;
}

int handle_add_char(void) 
{
    char c;
    printf("Введите букву для добавления: ");
    if (scanf(" %c", &c) != 1 || !isalpha((unsigned char)c)) {
        print_red("Неверная буква!\n");
        clear_input_buffer();
        return ERR_INVALID_INPUT;
    }
    clear_input_buffer();
    root = tree_insert(root, c);
    print_green("Буква добавлена.\n");
    return 0;
}

int handle_delete_char(void) 
{
    char c;
    printf("Введите букву для удаления: ");
    if (scanf(" %c", &c) != 1) {
        clear_input_buffer();
        return ERR_INVALID_INPUT;
    }
    clear_input_buffer();
    if (!tree_search(root, c)) {
        print_red("Буква не найдена!\n");
        return ERR_NOT_FOUND;
    }
    root = tree_delete(root, c);
    print_green("Буква удалена (одно вхождение).\n");
    return 0;
}

int handle_search_char(void) 
{
    char c;
    printf("Введите букву для поиска: ");
    if (scanf(" %c", &c) != 1) {
        clear_input_buffer();
        return ERR_INVALID_INPUT;
    }
    clear_input_buffer();
    if (tree_search(root, c))
        print_green("Буква '%c' найдена в дереве.\n", c);
    else
        print_red("Буква '%c' не найдена.\n", c);
    return 0;
}

// === Сравнение эффективности ===
void performance_test(const char *description, const char *str, int random_order) 
{
    tree_node_t *test_root = NULL;
    char temp[1024];
    strcpy(temp, str);

    if (random_order) 
    {
        for (int i = 0; temp[i]; i++) {
            if (isalpha((unsigned char)temp[i])) 
            {
                int j = rand() % (i + 1);
                char t = temp[i]; temp[i] = temp[j]; temp[j] = t;
            }
        }
    }

    clock_t start = clock();
    for (int i = 0; temp[i]; i++)
        if (isalpha((unsigned char)temp[i]))
            test_root = tree_insert(test_root, temp[i]);
    clock_t end = clock();
    double build_time = (end - start) / (double)CLOCKS_PER_SEC;

    start = clock();
    tree_search(test_root, 'a');  // пример поиска
    end = clock();
    double search_time = (end - start) / (double)CLOCKS_PER_SEC;

    printf("%s\n", description);
    printf("   Построение дерева: %.6f сек\n", build_time);
    printf("   Поиск (пример):    %.6f сек\n", search_time);
    tree_free(test_root);
}

int handle_performance_comparison(void) 
{
    if (!last_input_string[0]) {
        print_red("Сначала введите строку!\n");
        return ERR_INVALID_INPUT;
    }
    srand(time(NULL));
    print_yellow("\n=== Сравнение эффективности ===\n");
    performance_test("Случайный порядок (сбалансированное дерево)", last_input_string, 1);
    performance_test("Отсортированный порядок (вырожденное дерево)", last_input_string, 0);
    return 0;
}

void show_menu(void) 
{
    print_yellow("\n=== МЕНЮ ===\n");
    printf("1. Ввести строку и построить дерево\n");
    printf("2. Вывести дерево (дубликаты — красным)\n");
    printf("3. Удалить все повторяющиеся буквы\n");
    printf("4. Постфиксный обход (оставшиеся буквы)\n");
    printf("5. Сравнить время удаления дубликатов\n");
    printf("6. Префиксный обход\n");
    printf("7. Инфиксный обход\n");
    printf("8. Добавить букву\n");
    printf("9. Удалить букву\n");
    printf("10. Найти букву\n");
    printf("11. Сравнение эффективности (сбалансированное vs вырожденное)\n");
    printf("0. Выход\n");
    print_cyan("Выбор: ");
}

int main(void) 
{
    int choice;
    do 
    {
        show_menu();
        if (scanf("%d", &choice) != 1) 
        {
            print_red("Неверный ввод!\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) 
        {
            case 1: 
                handle_build_tree(); 
                break;
            case 2: 
                handle_print_tree_colored(); 
                break;
            case 3: 
                handle_remove_duplicates(); 
                break;
            case 4: 
                handle_postorder(); 
                break;
            case 5: 
                handle_compare_performance(); 
                break;
            case 6: 
                handle_preorder(); 
                break;
            case 7: 
                handle_inorder(); 
                break;
            case 8: 
                handle_add_char(); 
                break;
            case 9: 
                handle_delete_char(); 
                break;
            case 10: 
                handle_search_char(); 
                break;
            case 11: 
                handle_performance_comparison(); 
                break;
            case 0: 
                print_cyan("Завершение работы.\n"); 
                break;
            default: 
                print_red("Нет такого варианта ответа!\n");
        }
    } while (choice != 0);

    tree_free(root);
    return 0;
}
