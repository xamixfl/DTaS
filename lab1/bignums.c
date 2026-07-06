#include "bignums.h"

char* strip_leading_zeros(char *s) 
{
    while (*s == '0' && s[1] != '\0' && s[1] != '.') s++;
    return s;
}

void strip_trailing_zeros(char *s) 
{
    char *dot = strchr(s, '.');
    if (!dot) return;

    char *end = s + strlen(s) - 1;
    while (end > dot && *end == '0') 
    {
        *end = '\0';
        end--;
    }
    if (*end == '.') *end = '\0';
}

int cmp_num(const char *a, const char *b)
{
    while (*a == '0' && a[1]) a++;
    while (*b == '0' && b[1]) b++;

    int la = strlen(a), lb = strlen(b);
    if (la != lb) 
        return (la < lb) ? -1 : 1;

    return strcmp(a,b) < 0 ? -1 : (strcmp(a,b) > 0 ? 1 : 0);
}

void sub_num(char *a, const char *b)
{
    int la = strlen(a);
    int lb = strlen(b);
    int carry = 0;

    for (int i = 0; i < la; i++) 
    {
        int ai = a[la - 1 - i] - '0';
        int bi = (i < lb) ? b[lb - 1 - i] - '0' : 0;
        int d = ai - bi - carry;
        if (d < 0) 
        {
            d += 10; 
            carry = 1; 
        } 
        else 
            carry = 0;

        a[la - 1 - i] = d + '0';
    }

    char *s = strip_leading_zeros(a);
    if (s != a) 
        memmove(a, s, strlen(s) + 1);
}

void mul_num_digit(const char *a, int digit, char *res)
{
    if (digit==0) 
    { 
        strcpy(res, "0"); 
        return; 
    }
    if (digit == 1) 
    { 
        strcpy(res, a); 
        return; 
    }

    int la = strlen(a);
    int carry = 0;
    char buf[MAX_LEN + 5];
    buf[la + 1] = '\0';

    for (int i = la - 1; i >= 0; i--)
    {
        int prod = (a[i] - '0') * digit + carry;
        buf[i + 1] = prod % 10 + '0';
        carry = prod / 10;
    }

    if (carry) buf[0] = carry + '0';
    else memmove(buf, buf + 1, la + 1);
    
    strcpy(res, strip_leading_zeros(buf));
}

// Функция для правильного математического округления
void mathematical_round(char *number, int max_precision)
{
    char *dot = strchr(number, '.');
    if (!dot) return;
    
    char *fraction = dot + 1;
    int frac_len = strlen(fraction);
    
    if (frac_len <= max_precision) return;
    
    // Проверяем цифру для округления
    if (fraction[max_precision] >= '5') {
        // Округляем вверх
        int pos = max_precision - 1;
        while (pos >= 0) {
            if (fraction[pos] < '9') {
                fraction[pos]++;
                break;
            } else {
                fraction[pos] = '0';
                pos--;
            }
        }
        
        // Если все цифры дробной части стали 0
        if (pos < 0) {
            // Увеличиваем целую часть
            *dot = '\0'; // временно убираем точку
            char *int_part = number;
            int int_len = strlen(int_part);
            
            for (int i = int_len - 1; i >= 0; i--) {
                if (int_part[i] < '9') {
                    int_part[i]++;
                    break;
                } else {
                    int_part[i] = '0';
                    if (i == 0) {
                        memmove(number + 1, number, strlen(number) + 1);
                        number[0] = '1';
                        break;
                    }
                }
            }
            // Восстанавливаем как целое число (без дробной части)
            fraction[0] = '\0';
        }
    }
    
    // Обрезаем до нужной точности
    fraction[max_precision] = '\0';
    
    // Убираем конечные нули
    strip_trailing_zeros(number);
}

void debug_division(const char *dividend, const char *divisor, const char *result)
{
    printf("Dividend: %s\n", dividend);
    printf("Divisor:  %s\n", divisor);
    printf("Result:   %s\n", result);
    printf("---\n");
}

int divide_strings(const char *dividend_in, const char *divisor_in, char *result) {
    char divisor[MAX_RESULT] = {0};
    char dividend[MAX_RESULT * 2] = {0};
    char rem[MAX_RESULT * 2] = {0};
    char prod[MAX_RESULT * 2] = {0};
    char res_digits[MAX_RESULT] = {0};
    
    const int max_precision = 50;
    
    // Убираем точки из чисел
    int divisor_scale = 0, dividend_scale = 0;
    int j = 0;
    
    // Обрабатываем делитель
    for (size_t i = 0; i < strlen(divisor_in); i++) {
        if (divisor_in[i] == '.') {
            divisor_scale = strlen(divisor_in) - i - 1;
            continue;
        }
        divisor[j++] = divisor_in[i];
    }
    divisor[j] = '\0';
    
    j = 0;
    // Обрабатываем делимое
    for (size_t i = 0; i < strlen(dividend_in); i++) {
        if (dividend_in[i] == '.') {
            dividend_scale = strlen(dividend_in) - i - 1;
            continue;
        }
        dividend[j++] = dividend_in[i];
    }
    dividend[j] = '\0';
    
    strcpy(divisor, strip_leading_zeros(divisor));
    strcpy(dividend, strip_leading_zeros(dividend));
    
    if (strcmp(divisor, "0") == 0) return ERROR_DIVISION;
    
    // Добавляем нули для компенсации масштаба
    int total_scale = dividend_scale - divisor_scale;
    if (total_scale > 0) {
        int len = strlen(dividend);
        for (int i = 0; i < total_scale; i++) {
            dividend[len + i] = '0';
        }
        dividend[len + total_scale] = '\0';
    }
    
    // Начинаем деление
    strcpy(rem, "0");
    int res_len = 0;
    int decimal_point_pos = -1;
    
    // Обрабатываем целую часть
    for (size_t i = 0; i < strlen(dividend); i++) {
        int lr = strlen(rem);
        rem[lr] = dividend[i];
        rem[lr + 1] = '\0';
        strcpy(rem, strip_leading_zeros(rem));
        
        int digit = 0;
        for (int d = 9; d >= 0; d--) {
            mul_num_digit(divisor, d, prod);
            if (cmp_num(prod, rem) <= 0) {
                digit = d;
                break;
            }
        }
        
        res_digits[res_len++] = digit + '0';
        
        if (digit > 0) {
            mul_num_digit(divisor, digit, prod);
            sub_num(rem, prod);
        }
    }
    
    // Добавляем десятичную точку
    decimal_point_pos = res_len;
    
    // Обрабатываем дробную часть
    if (strcmp(rem, "0") != 0) {
        int fraction_digits = 0;
        while (fraction_digits < max_precision + 2 && strcmp(rem, "0") != 0) {
            int lr = strlen(rem);
            rem[lr] = '0';
            rem[lr + 1] = '\0';
            strcpy(rem, strip_leading_zeros(rem));
            
            int digit = 0;
            for (int d = 9; d >= 0; d--) {
                mul_num_digit(divisor, d, prod);
                if (cmp_num(prod, rem) <= 0) {
                    digit = d;
                    break;
                }
            }
            
            res_digits[res_len++] = digit + '0';
            fraction_digits++;
            
            if (digit > 0) {
                mul_num_digit(divisor, digit, prod);
                sub_num(rem, prod);
            }
        }
    }
    
    // Формируем результат с точкой
    int pos = 0;
    for (int i = 0; i < res_len; i++) {
        if (i == decimal_point_pos) {
            result[pos++] = '.';
        }
        result[pos++] = res_digits[i];
    }
    result[pos] = '\0';
    
    // Применяем математическое округление
    mathematical_round(result, max_precision);
    
    debug_division(dividend_in, divisor_in, result);

    return 0;
}


void normalize_result(char *num, int *exp) 
{
    int len = (int)strlen(num);
    
    // Убираем ведущие нули
    char *start = num;
    while (*start == '0' && start[1] != '.' && start[1] != '\0') start++;
    if (start != num) {
        memmove(num, start, strlen(start) + 1);
        len = strlen(num);
    }
    
    // Находим позицию точки
    int dot_pos = -1;
    for (int i = 0; i < len; i++) {
        if (num[i] == '.') {
            dot_pos = i;
            break;
        }
    }
    
    // Если точки нет, добавляем её в конец
    if (dot_pos == -1) {
        num[len] = '.';
        num[len + 1] = '0';
        num[len + 2] = '\0';
        dot_pos = len;
        len += 2;
    }
    
    // Находим первую ненулевую цифру
    int first_non_zero = -1;
    for (int i = 0; i < len; i++) {
        if (num[i] != '0' && num[i] != '.') {
            first_non_zero = i;
            break;
        }
    }
    
    // Если все цифры нули
    if (first_non_zero == -1) {
        strcpy(num, "0.0");
        *exp = 0;
        return;
    }
    
    // Вычисляем экспоненту
    if (first_non_zero < dot_pos) {
        // Цифры слева от точки
        *exp = dot_pos - first_non_zero;
    } else {
        // Цифры справа от точки
        *exp = dot_pos - first_non_zero + 1;
    }
    
    // Формируем нормализованную мантиссу
    char normalized[MAX_LEN] = "0.";
    int norm_pos = 2;
    
    // Копируем значащие цифры
    for (int i = first_non_zero; i < len && norm_pos < MAX_LEN - 1; i++) {
        if (num[i] != '.') {
            normalized[norm_pos++] = num[i];
        }
    }
    normalized[norm_pos] = '\0';
    
    // Убираем конечные нули
    strip_trailing_zeros(normalized);
    
    strcpy(num, normalized);
}


int big_division(bignum_t num1, bignum_t num2, bignum_t *res)                                    // Выполнение функций
{
    int exp;
    normalize_result(num2.mantiss, &exp); // нормализуем до рассчета

    res->negative = num1.negative ^ num2.negative;
    res->grade = num2.grade * (-1);

    int rc = divide_strings(num1.mantiss, num2.mantiss, res->mantiss);
    if (rc != 0)
    {
        printf("На ноль делить нельзя!\n");
        return ERROR_DIVISION;
    }

    int exp_res;
    normalize_result(res->mantiss, &exp_res);   // нормализуем после

    //printf("%d %d %d \n", num2.grade , exp, exp_res);
    //res->grade = 0;
    //printf("%d %d\n", res->grade, exp);
    res->grade -= exp;     // суммируем степени 10
    res->grade += exp_res;
    //printf("%d %d\n", res->grade, exp);

    //printf("%d %d %d \n", res->grade, exp);

    if (abs(res->grade) > MAX_GRADE)
    {
        printf("Переполнение.\n");
        return ERROR_OVERFLOW;
    }    

    return 0;
}
