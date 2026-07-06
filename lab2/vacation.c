#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vacation.h"

// Безопасно считывает строку из стандартного ввода.
static int read_line(char *buffer, int size)
{
    if (fgets(buffer, size, stdin) == NULL)
        return ERROR_INPUT;

    char *newline = strchr(buffer, '\n');

    if (newline != NULL)
        *newline = '\0';
    else
        return ERROR_TOO_LONG;

    return 0;
}

//Очищает буфер стандартного ввода.
static void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Добавляет новую страну в таблицу.
int add_vacation(vacation_t *table, size_t *size)
{
    if (*size >= TABLE_SIZE)
        return ERROR_FULL_TABLE;

    int rc;
    vacation_t *v = &table[*size];

    printf("Введите страну: ");
    if ((rc = read_line(v->country, STR_LEN))) return rc;
    
    printf("Введите столицу: ");
    if ((rc = read_line(v->capital, STR_LEN))) return rc;

    printf("Введите материк (0-Азия, 1-Европа, 2-Юж.Америка, 3-Сев.Америка, 4-Австралия, 5-Антарктида): ");
    int continent_input;
    if (scanf("%d", &continent_input) != 1 || continent_input < 0 || continent_input > 5)
        return ERROR_INPUT;
    v->continent = (Continent_t)continent_input;
    clear_input_buffer();

    printf("Введите время полета (часы минуты): ");
    if (scanf("%d %d", &v->time.hours, &v->time.minutes) != 2 || 
        v->time.hours < 0 || v->time.minutes < 0 || v->time.minutes > 59)
    {
        clear_input_buffer();
        return ERROR_INPUT;
    }
        
    clear_input_buffer();

    printf("Нужна ли виза? (0 - нет, 1 - да): ");
    int visa_input;
    if (scanf("%d", &visa_input) != 1 || (visa_input != 0 && visa_input != 1))
        return ERROR_INPUT;
    v->visa = (bool)visa_input;
    clear_input_buffer();

    printf("Введите минимальную стоимость поездки: ");
    if (scanf("%lf", &v->cost) != 1 || v->cost < 0)
        return ERROR_INPUT;
    clear_input_buffer();

    printf("Введите тип отдыха (0-экскурсионный, 1-пляжный, 2-спортивный): ");
    int holiday_input;
    if (scanf("%d", &holiday_input) != 1 || holiday_input < 0 || holiday_input > 2)
        return ERROR_INPUT;
    v->holiday_type = (HolidaysType_t)holiday_input;
    clear_input_buffer();

    switch (v->holiday_type)
    {
        case EXCURSION:
            printf("Количество объектов: ");
            if (scanf("%d", &v->tourism.excursion.objects_amount) != 1 || v->tourism.excursion.objects_amount < 0)
                return ERROR_INPUT;
            clear_input_buffer();
            
            printf("Тип объектов (0-исторические, 1-искусство, 2-природа): ");
            int objects_input;
            if (scanf("%d", &objects_input) != 1 || objects_input < 0 || objects_input > 2)
                return ERROR_INPUT;
            v->tourism.excursion.object_type = (Objects_t)objects_input;
            clear_input_buffer();
            break;
            
        case BEACH:
            printf("Сезон (0-весна, 1-лето, 2-осень, 3-зима): ");
            int season_input;
            if (scanf("%d", &season_input) != 1 || season_input < 0 || season_input > 3)
                return ERROR_INPUT;
            v->tourism.beach.season = (Season_t)season_input;
            clear_input_buffer();
            
            printf("Температура воды: ");
            if (scanf("%lf", &v->tourism.beach.water_temp) != 1)
                return ERROR_INPUT;
            clear_input_buffer();
            
            printf("Температура воздуха: ");
            if (scanf("%lf", &v->tourism.beach.air_temp) != 1)
                return ERROR_INPUT;
            clear_input_buffer();
            break;
            
        case SPORT:
            printf("Вид спорта: ");
            if ((rc = read_line(v->tourism.sport.sport, STR_LEN))) return rc;
            //clear_input_buffer();
            break;
    }

    (*size)++;
    printf("Страна успешно добавлена в таблицу.\n");

    return 0;
}

//Удаляет страну из таблицы по названию.
int delete_vacation(vacation_t *table, size_t *size)
{
    if (*size == 0)
        return ERROR_EMPTY_TABLE;

    int rc;
    char country[STR_LEN];
    printf("Введите название страны для удаления: ");
    if ((rc = read_line(country, STR_LEN))) return rc;

    size_t i = 0;
    bool deleted = false;
    while (i < *size)
    {
        if (strcmp(table[i].country, country) == 0)
        {
            for (size_t j = i; j < *size - 1; j++)
                table[j] = table[j + 1];
            (*size)--;
            deleted = true;
            break;
        }
        else
            i++;
    }

    if (deleted)
        printf("Страна %s успешно удалена из таблицы.\n", country);
    else
        printf("Страна %s в таблице не найдена.\n", country);

    return 0;
}

void print_table(const vacation_t *table, const size_t size)
{
    if (size == 0)
        return;

    printf("\n| %-3s | %-16s | %-16s | %-12s | %-12s | %-18s | %-15s | %-16s | %-40s |\n",
           "№", "Страна", "Столица", "Материк", "Время полета", "Необходимость визы", 
           "Минимальная цена", "Вид туризма", "Описание вида");
    printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    for (size_t i = 0; i < size; i++)
    {
        const vacation_t *v = &table[i];
        
        // Преобразование времени полета
        char time_str[10];
        snprintf(time_str, sizeof(time_str), "%02d:%02d", v->time.hours, v->time.minutes);
        
        // Преобразование материка
        const char *continent_str;
        switch (v->continent)
        {
            case ASIA: continent_str = "Азия"; break;
            case EUROPE: continent_str = "Европа"; break;
            case SOUTH_AMERICA: continent_str = "Юж.Америка"; break;
            case NORTH_AMERICA: continent_str = "Сев.Америка"; break;
            case AUSTRALIA: continent_str = "Австралия"; break;
            case ANTARTICA: continent_str = "Антарктида"; break;
            default: continent_str = "Неизвестно"; break;
        }
        
        // Преобразование необходимости визы
        const char *visa_str = v->visa ? "Да" : "Нет";

        printf("| %-3zu | %-16s | %-16s | %-12s | %-12s | %-18s | %-15.2f | ",
               i + 1, v->country, v->capital, continent_str, time_str, visa_str, v->cost);

        // Вывод информации о виде туризма
        char description[100];
        switch (v->holiday_type)
        {
            case EXCURSION:
                printf("%-16s | ", "Экскурсионный");
                // Преобразование типа объектов
                const char *objects_str;
                switch (v->tourism.excursion.object_type)
                {
                    case HISTORY: objects_str = "Исторические"; break;
                    case ART: objects_str = "Искусство"; break;
                    case NATURE: objects_str = "Природа"; break;
                    default: objects_str = "Неизвестно"; break;
                }
                snprintf(description, sizeof(description), "Объектов: %d, Тип: %s", 
                        v->tourism.excursion.objects_amount, objects_str);
                break;
                
            case BEACH:
                printf("%-16s | ", "Пляжный");
                // Преобразование сезона
                const char *season_str;
                switch (v->tourism.beach.season)
                {
                    case SPRING: season_str = "Весна"; break;
                    case SUMMER: season_str = "Лето"; break;
                    case AUTUMN: season_str = "Осень"; break;
                    case WINTER: season_str = "Зима"; break;
                    default: season_str = "Неизвестно"; break;
                }
                snprintf(description, sizeof(description), "Сезон: %s, Воздух: %.1f°C, Вода: %.1f°C", 
                       season_str, v->tourism.beach.air_temp, v->tourism.beach.water_temp);
                break;
                
            case SPORT:
                printf("%-16s | ", "Спортивный");
                snprintf(description, sizeof(description), "Вид спорта: %s", v->tourism.sport.sport);
                break;
                
            default:
                printf("%-16s | ", "Неизвестно");
                strcpy(description, "");
                break;
        }
        
        printf("%-40s |\n", description);
    }
    printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}



//Выводит на экран таблицу стран в порядке, заданном таблицей ключей.
void print_by_keys(const vacation_t *table, const KeyEntry_t *keys, size_t size)
{
    if (size == 0)
        return;

    printf("\n--- Таблица в порядке ключей (страны) ---\n");
    printf("| %-3s | %-6s | %-15s | %-15s | %-10s | %-12s | %-18s | %-15s | %-20s | %-30s |\n",
           "№", "Индекс", "Страна", "Столица", "Материк", "Время полета", "Необходимость визы", "Минимальная цена", "Основной вид туризма", "Описание вида");
    printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    for (size_t i = 0; i < size; i++)
    {
        int idx = keys[i].original_index;
        const vacation_t *v = &table[idx];
        
        // Преобразование времени полета
        char time_str[10];
        snprintf(time_str, sizeof(time_str), "%02d:%02d", v->time.hours, v->time.minutes);
        
        // Преобразование материка
        const char *continent_str;
        switch (v->continent)
        {
            case ASIA: continent_str = "Азия"; break;
            case EUROPE: continent_str = "Европа"; break;
            case SOUTH_AMERICA: continent_str = "Юж.Америка"; break;
            case NORTH_AMERICA: continent_str = "Сев.Америка"; break;
            case AUSTRALIA: continent_str = "Австралия"; break;
            case ANTARTICA: continent_str = "Антарктида"; break;
            default: continent_str = "Неизвестно"; break;
        }
        
        // Преобразование необходимости визы
        const char *visa_str = v->visa ? "Да" : "Нет";

        printf("| %-3zu | %-6d | %-15s | %-15s | %-10s | %-12s | %-18s | %-15.2f | ",
               i + 1, keys[i].original_index + 1, v->country, v->capital, continent_str, time_str, visa_str, v->cost);

        // Вывод информации о виде туризма
        switch (v->holiday_type)
        {
            case EXCURSION:
                printf("%-20s | ", "Экскурсионный");
                // Преобразование типа объектов
                const char *objects_str;
                switch (v->tourism.excursion.object_type)
                {
                    case HISTORY: objects_str = "Исторические"; break;
                    case ART: objects_str = "Искусство"; break;
                    case NATURE: objects_str = "Природа"; break;
                    default: objects_str = "Неизвестно"; break;
                }
                printf("Объектов: %d, Тип: %-15s |", v->tourism.excursion.objects_amount, objects_str);
                break;
                
            case BEACH:
                printf("%-20s | ", "Пляжный");
                // Преобразование сезона
                const char *season_str;
                switch (v->tourism.beach.season)
                {
                    case SPRING: season_str = "Весна"; break;
                    case SUMMER: season_str = "Лето"; break;
                    case AUTUMN: season_str = "Осень"; break;
                    case WINTER: season_str = "Зима"; break;
                    default: season_str = "Неизвестно"; break;
                }
                printf("Сезон: %s, Воздух: %.1f°C, Вода: %.1f°C |", 
                       season_str, v->tourism.beach.air_temp, v->tourism.beach.water_temp);
                break;
                
            case SPORT:
                printf("%-20s | ", "Спортивный");
                printf("Вид спорта: %-20s |", v->tourism.sport.sport);
                break;
                
            default:
                printf("%-20s | %-30s |", "Неизвестно", "");
                break;
        }
        printf("\n");
    }
    printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

//Загружает в таблицу предопределенный набор тестовых данных.
void load_test_data(vacation_t *table, size_t *size)
{
    *size = 0;
    vacation_t v;
    
    char *countries[] = {"Россия", "Франция", "Италия", "Испания", "Германия", "Турция", "Египет", "Греция", "Китай", "Япония", 
                        "США", "Канада", "Бразилия", "Аргентина", "Австралия", "Новая Зеландия", "Таиланд", "Вьетнам", "Индия", "ОАЭ",
                        "Мексика", "Южная Корея", "Индонезия", "Великобритания", "Португалия", "Швейцария", "Норвегия", "Финляндия",
                        "Чехия", "Польша", "Венгрия", "Австрия", "Бельгия", "Нидерланды", "Дания", "Швеция", "Ирландия", "Сингапур",
                        "Малайзия", "Филиппины"};
    
    char *capitals[] = {"Москва", "Париж", "Рим", "Мадрид", "Берлин", "Анкара", "Каир", "Афины", "Пекин", "Токио",
                       "Вашингтон", "Оттава", "Бразилиа", "Буэнос-Айрес", "Канберра", "Веллингтон", "Бангкок", "Ханой", "Дели", "Абу-Даби",
                       "Мехико", "Сеул", "Джакарта", "Лондон", "Лиссабон", "Берн", "Осло", "Хельсинки", "Прага", "Варшава",
                       "Будапешт", "Вена", "Брюссель", "Амстердам", "Копенгаген", "Стокгольм", "Дублин", "Сингапур", "Куала-Лумпур", "Манила"};
    
    char *sports[] = {"горные лыжи", "сноуборд", "дайвинг", "серфинг", "яхтинг", "гольф", "теннис", "альпинизм", "рафтинг", "велотуризм",
                     "футбол", "баскетбол", "волейбол", "плавание", "бег", "бокс", "единоборства", "йога", "фитнес", "скалолазание"};

    for (int i = 0; i < 40; i++)  // изменено с 20 на 40
    {
        // Основная информация о стране
        strcpy(v.country, countries[i % 40]);  // изменено с 20 на 40
        strcpy(v.capital, capitals[i % 40]);   // изменено с 20 на 40
        v.continent = (Continent_t)(rand() % 6); // 6 материков
        
        // Время полета (от 1 до 15 часов)
        v.time.hours = 1 + rand() % 15;
        v.time.minutes = (rand() % 4) * 15; // 0, 15, 30 или 45 минут (более реалистично)
        
        // Виза (случайно)
        v.visa = (rand() % 3 == 0); // 33% шанс что виза нужна
        
        // Стоимость (от 50000 до 300000 рублей)
        v.cost = 50000 + rand() % 250000;
        
        // Тип отдыха - распределение более равномерное
        v.holiday_type = (HolidaysType_t)(i % 3); // чередование типов для разнообразия
        
        // Заполнение данных в зависимости от типа отдыха
        switch (v.holiday_type)
        {
            case EXCURSION:
                v.tourism.excursion.objects_amount = 5 + rand() % 20; // от 5 до 25 объектов
                v.tourism.excursion.object_type = (Objects_t)(rand() % 3); // тип объектов
                break;
                
            case BEACH:
                v.tourism.beach.season = (Season_t)(rand() % 4); // сезон
                v.tourism.beach.water_temp = 15.0 + (double)(rand() % 250) / 10.0; // от 15°C до 40°C
                v.tourism.beach.air_temp = v.tourism.beach.water_temp + (double)(rand() % 50) / 10.0 - 2.5; // ±2.5°C от температуры воды
                break;
                
            case SPORT:
                strcpy(v.tourism.sport.sport, sports[rand() % 20]); // изменено с 10 на 20
                break;
        }
        
        table[*size] = v;
        (*size)++;
    }
    printf("%zu тестовых записей об отдыхе успешно загружено.\n", *size);
}

//Осуществляет поиск стран по заданным варианту.
int search_country_by_options(const vacation_t *table, const size_t size)
{
    int continent_input;  
    char sport[STR_LEN];
    double max_cost;
    
    printf("--- Поиск стран на выбранном материке с указанным видом спорта и ценой ---\n");
    
    printf("Введите материк (0-Азия, 1-Европа, 2-Юж.Америка, 3-Сев.Америка, 4-Австралия, 5-Антарктида): ");
    if (scanf("%d", &continent_input) != 1 || continent_input < 0 || continent_input > 5)
        return ERROR_INPUT;
    clear_input_buffer();

    Continent_t continent;
    switch(continent_input)
    {
        case 0:
            continent = ASIA;
            break;
        case 1:
            continent = EUROPE;
            break;
        case 2:
            continent = SOUTH_AMERICA;
            break;
        case 3:
            continent = NORTH_AMERICA;
            break;
        case 4:
            continent = AUSTRALIA;
            break;
        case 5:
            continent = ANTARTICA;
            break;
    }

    printf("Введите вид спорта: ");
    if (read_line(sport, STR_LEN) != 0) return ERROR_INPUT;

    printf("Введите максимальную стоимость поездки: ");
    if (scanf("%lf", &max_cost) != 1 || max_cost < 0) return ERROR_INPUT;
    clear_input_buffer();

    vacation_t results[TABLE_SIZE];
    int results_count = 0;

    for (size_t i = 0; i < size; i++)
    {
        if (table[i].holiday_type == SPORT &&
            table[i].continent == continent && 
            strcmp(table[i].tourism.sport.sport, sport) == 0 &&
            table[i].cost <= max_cost)
        {
            results[results_count] = table[i];
            results_count++;
        }
    }

    if (results_count > 0)
    {
        printf("\n--- Результаты поиска ---\n");
        print_table(results, results_count);
    }
    else
        printf("\nСтраны, удовлетворяющие критериям, не найдены.\n");

    return 0;
}

//Выводит на экран таблицу ключей.
/*void print_key_table(const KeyEntry_t *keys, size_t size)
{
    if (size == 0)
    {
        printf("Таблица ключей пуста.\n");
        return;
    }

    printf("\n--- Таблица ключей ---\n");
    printf("| %-5s | %-15s | %-20s |\n", "№ п/п", " Страна (ключ)", "Индекс в осн. табл.");
    printf("--------------------------------------------------\n");

    for (size_t i = 0; i < size; i++)
    {
        printf("| %-5zu | %-15s | %-20d |\n",
               i + 1, keys[i].country, keys[i].original_index + 1);
    }
    printf("--------------------------------------------------\n");
}*/

//Выводит на экран таблицу ключей.
void print_key_table(const KeyEntry_t *keys, size_t size)
{
    if (size == 0)
    {
        printf("Таблица ключей пуста.\n");
        return;
    }

    printf("\n--- Таблица ключей ---\n");
    printf("| %-5s | %-20s | %-20s |\n", "№ п/п", "Страна (ключ)", "Индекс в осн. табл.");
    printf("------------------------------------------------------------\n");

    for (size_t i = 0; i < size; i++)
    {
        // Обрезаем длинные названия стран
        char country_display[STR_LEN];
        /*if (strlen(keys[i].country) > 20)
        {
            strncpy(country_display, keys[i].country, 17);
            strcpy(country_display + 17, "...");
        }*/
        strncpy(country_display, keys[i].country, STR_LEN - 1);

        printf("| %-5zu | %-20s | %-20d |\n",
               i + 1, country_display, keys[i].original_index + 1);
    }
    printf("------------------------------------------------------------\n");
}


