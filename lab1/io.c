#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h> 

#include "io.h"

int input_int(char *intnum, bool *neg)
{
    int len = 0;
    char c;
    bool sign = false;

    while (scanf("%c", &c) == 1 && isspace(c)) {}

    if (c == '-' || c == '+')
    {
        *neg = (c == '-' ? true : false);
        sign = true;
    }
    if (sign)
    {
        if (scanf("%c", &c) != 1)
        {
            printf("Ошибка ввода.\n");
            return ERROR_INPUT;
        }  
    }

    while (c == '0')
    {
        if (scanf("%c", &c) != 1)
        {
            printf("Ошибка ввода.\n");
            return ERROR_INPUT;
        }
    }
    if (isdigit(c))
    {
        intnum[len] = c;
        len++;
    }
    else if (c == '\n')
    {
        intnum[len] = '0';
        len++;
        return 0;
    }
    else
    {
        printf("Ошибка ввода.\n");
        return ERROR_INPUT;
    }

    while (scanf("%c", &c) == 1 && isdigit(c) && len < MAX_LEN - 1)        // Проверяем, что символов не более 40
    {
        if (len == MAX_LEN - 2)
        {
            printf("Ошибка ввода: слишком много символов.\n");
            return ERROR_INPUT;
        }
        intnum[len] = c;
        len++;
    }

    while (isspace(c) && c != '\n') 
    {
        if (scanf("%c", &c) != 1)
            break;
    }

    if (c != '\n')
    {
        printf("Ошибка ввода.\n");
        return ERROR_INPUT;
    }

    intnum[len] = '\0';
    return 0;
}

int input_float(char *mantiss, int *grade, bool *neg)
{
    char c;
    bool has_digits = false;
    *grade = 0;
    int state = 0; // 0 - ввод целой части, 1 - ввод дробной части, 2 - ввод экспоненциального порядка;
    
    int len = 0;
    
    while (scanf("%c", &c) == 1 && isspace(c)) {}

    if (c == '-' || c == '+') 
    {
        *neg = (c == '-' ? true : false);
        
        if (scanf("%c", &c) != 1) 
        {
            printf("Ошибка ввода после знака.\n");
            return ERROR_INPUT;
        }
    }

    while (c == ' ')
    {
        if (scanf("%c", &c) != 1)
        {
            printf("Ошибка ввода.\n");
            return ERROR_INPUT;
        }
    } 

    while (c == '0')
    {
        has_digits = true;
        if (scanf("%c", &c) != 1)
        {
            printf("Ошибка ввода.\n");
            return ERROR_INPUT;
        }
    } 

    if (c == '.')
    {
        mantiss[len++] = '0';
        mantiss[len++] = '.';
        state = 1;
        has_digits = true;

        if (scanf("%c", &c) != 1)
        {
            printf("Ошибка ввода.\n");
            return ERROR_INPUT;
        }
    }

    while (len < MAX_LEN + 1)                                          // основной цикл + проверка на количество символов (<= 40)
    {
        //printf("%d", len);
        if (c == ' ')
        {
            if (scanf("%c", &c) != 1)
            {
                break;
            }
            if (c == 'E')
            {
                state = 2;
                has_digits = true;
                if (scanf("%d", grade) != 1 || abs(*grade) >= 100000)
                {
                    printf("Ошибка ввода. Некорректная степень.\n");
                    return ERROR_INPUT;
                }
                //printf("%d \n", *grade);
            }
            else 
            {
                printf("Ошибка ввода: недопустимый символ '%c'.\n", c);
                return ERROR_INPUT;
            }
        }
        else if (c == '\n')
        {
            break;
        }
        else if (len >= MAX_LEN)
        {
            printf("Ошибка ввода: слишком много символов.\n");
            return ERROR_INPUT;
        }
        else if ((len == (state == 0 ? MAX_LEN - 2 : MAX_LEN - 1)) && !(mantiss[0] == '0' && mantiss[1] == '.'))   // && c != '\n')
        {
            printf("Ошибка ввода: слишком много символов.\n");
            return ERROR_INPUT;
        }
        else if (isdigit(c)) 
        {
            mantiss[len++] = c;
            has_digits = true;
        }
        else if (c == '.' && state == 0) 
        {
            mantiss[len++] = '.';
            state = 1;
        }
        else 
        {
            printf("Ошибка ввода: недопустимый символ '%c'.\n", c);
            return ERROR_INPUT;
        }
        
        // Читаем следующий символ
        if (scanf("%c", &c) != 1) 
        {
            break;
        }
    }
    
    if (!has_digits)                                                                                 // Проверка корректности числа
    {
        printf("Ошибка ввода: число не содержит цифр.\n");
        return ERROR_INPUT;
    }
    
    if (state != 0 && mantiss[len - 1] == '.') 
    {
        if (len < MAX_LEN - 1) 
        {
            mantiss[len++] = '0';
        }
    }

    if  (len == 0)
    {
        mantiss[len++] = '0';
        printf("На ноль делить нельзя!\n");
        return ERROR_DIVISION;
    }
    
    mantiss[len] = '\0';
    return 0;
}

void print_result(bignum_t result)
{
    if (result.negative)
    {
        printf("- %s ", result.mantiss);
    }
    else
    {
        printf("%s ", result.mantiss);
    }
    
    printf("E%d\n", result.grade);

    //printf("%ld\n", strlen(result.mantiss));

    /*if (result.grade != 0 && strcmp(result.mantiss, "0.0"))
    {
        printf("E%d\n", result.grade);
    }
    else
        printf("\n");*/
}
