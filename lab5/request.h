#ifndef REQUEST_H
#define REQUEST_H

#define MAX_REQUESTS 2000   

typedef struct 
{
    int type;             
    double arrival_time;     
    double service_start_time;
} request_t;

// Глобальный массив всех заявок
extern request_t requests[MAX_REQUESTS];

#endif
