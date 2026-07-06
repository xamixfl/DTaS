#ifndef SIMULATION_H
#define SIMULATION_H

#define REQUESTS_COUNT 1000

#define ERROR_TIME_LIMIT_EXCEEDED -5

// Структура для хранения параметров симуляции
typedef struct {
    double t1_min;  
    double t1_max;     
    double t2_min;      
    double t2_max;      
    double t3_min;      
    double t3_max;      
} simulation_params_t;

int simulation_run_array(const simulation_params_t current_params);
int simulation_run_list(const simulation_params_t current_params);

#endif
