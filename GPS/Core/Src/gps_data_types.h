#ifndef GPS_DATA_TYPES_H
#define GPS_DATA_TYPES_H

#include "main.h"

// Định nghĩa cấu trúc dữ liệu GPS
typedef struct Neo6M_GpsData
{
    double time;
    double latitude;
    double longitude;
    double Knots;
    double Date;
    char N_OR_S;
    char E_OR_W;
    char Data[750];
    char buffer[100];
    char location[100];
    char plusCode[20];  // Thêm trường mới để lưu Plus Code

    UART_HandleTypeDef UartPort;
} Neo6M_GpsData;

#endif /* GPS_DATA_TYPES_H */