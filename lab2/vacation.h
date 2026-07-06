#ifndef COUNTRY_H
#define COUNTRY_H

#define ERROR_INPUT 2
#define ERROR_TOO_LONG 3
#define ERROR_FULL_TABLE 4
#define ERROR_EMPTY_TABLE 5

#define STR_LEN 50
#define TABLE_SIZE 50

typedef enum
{
    ASIA,  
    EUROPE,
    SOUTH_AMERICA,
    NORTH_AMERICA,
    AUSTRALIA,
    ANTARTICA
} Continent_t;

typedef struct
{
    int hours;  
    int minutes;    
} Time_t;

// Перечисление для типа отдыха
typedef enum
{
    EXCURSION,  
    BEACH,   
    SPORT 
} HolidaysType_t;

// Перечисление для типов объектов (вынесено наружу)
typedef enum
{
    HISTORY,  
    ART,   
    NATURE 
} Objects_t;

// Перечисление для сезонов (вынесено наружу)
typedef enum
{
    SPRING,  // исправлено sping
    SUMMER, 
    AUTUMN, 
    WINTER
} Season_t;

// Объединение для хранения информации об отдыхе
typedef union
{
    struct
    {
        int objects_amount;
        Objects_t object_type;  // используем вынесенный enum
    } excursion; // Экскурсионный отдых
    
    struct
    {
        Season_t season;  // используем вынесенный enum
        double water_temp;
        double air_temp;
    } beach; // Пляжный отдых
    
    struct
    {
        char sport[STR_LEN];
    } sport; // Спортивный отдых 
} Tourism_t;


//Структура записи об отдыхе.
typedef struct
{
    char country[STR_LEN];
    char capital[STR_LEN];
    Continent_t continent;
    Time_t time;
    bool visa; // 0 - не нужна; 1 - нужна.
    double cost;
    HolidaysType_t holiday_type;
    Tourism_t tourism;

} vacation_t;

//Структура для элемента таблицы ключей.

typedef struct
{
    char country[STR_LEN];
    int original_index;
} KeyEntry_t;

int add_vacation(vacation_t *table, size_t *size);

int delete_vacation(vacation_t *table, size_t *size);

void print_table(const vacation_t *table, size_t size);

void print_by_keys(const vacation_t *table, const KeyEntry_t *keys, size_t size);

void load_test_data(vacation_t *table, size_t *size);

int search_country_by_options(const vacation_t *table, size_t size);

void print_key_table(const KeyEntry_t *keys, size_t size);

#endif