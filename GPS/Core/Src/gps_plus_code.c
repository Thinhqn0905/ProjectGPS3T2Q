#include "main.h"
#include "ssd1306.h"
#include "fonts.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "gps_data_types.h"


// Hàm để hiển thị dữ liệu GPS trên màn hình OLED
void Display_GPS_Data_On_OLED(Neo6M_GpsData* GpsData)
{
    char lat_str[20];
    char lon_str[20];
    char time_str[20];
    char date_str[20];
    char speed_str[20];
    char status_str[50];
    
    // Khởi tạo màn hình OLED
    SSD1306_Init();
    SSD1306_Clear();
    
    // Hiển thị tiêu đề
    SSD1306_GotoXY(0, 0);
    SSD1306_Puts("GPS TRACKER", &Font_7x10, SSD1306_COLOR_WHITE);
    SSD1306_DrawLine(0, 11, 128, 11, SSD1306_COLOR_WHITE);
    
    // Kiểm tra độ chính xác của dữ liệu
    if (GpsData->latitude != 0.0 && GpsData->longitude != 0.0) {
        // Định dạng dữ liệu vĩ độ và kinh độ
        snprintf(lat_str, sizeof(lat_str), "Lat: %.5f%c", fabs(GpsData->latitude), 
                 GpsData->N_OR_S);
        snprintf(lon_str, sizeof(lon_str), "Lon: %.5f%c", fabs(GpsData->longitude), 
                 GpsData->E_OR_W);
        
        // Định dạng dữ liệu thời gian (từ HHMMSS.SS sang HH:MM:SS)
        int hours = (int)(GpsData->time / 10000.0);
        int mins = (int)((GpsData->time - hours * 10000.0) / 100.0);
        int secs = (int)(GpsData->time - hours * 10000.0 - mins * 100.0);
        snprintf(time_str, sizeof(time_str), "Time: %02d:%02d:%02d", hours, mins, secs);
        
        // Tốc độ (chuyển từ knots sang km/h)
        float speed_kmh = GpsData->Knots * 1.852;  // 1 knot = 1.852 km/h
        snprintf(speed_str, sizeof(speed_str), "Speed: %.1f km/h", speed_kmh);
        
        // Định dạng Plus Code tạm thời (không thể tính toán chính xác Plus Code trên MCU)
        snprintf(status_str, sizeof(status_str), "Near %.5f,%.5f", 
                 GpsData->latitude, GpsData->longitude);
                
        // Hiển thị tất cả thông tin
        SSD1306_GotoXY(0, 15);
        SSD1306_Puts(lat_str, &Font_7x10, SSD1306_COLOR_WHITE);
        
        SSD1306_GotoXY(0, 27);
        SSD1306_Puts(lon_str, &Font_7x10, SSD1306_COLOR_WHITE);
        
        SSD1306_GotoXY(0, 39);
        SSD1306_Puts(time_str, &Font_7x10, SSD1306_COLOR_WHITE);
        
        SSD1306_GotoXY(0, 51);
        SSD1306_Puts(speed_str, &Font_7x10, SSD1306_COLOR_WHITE);
    } else {
        // Hiển thị thông báo đang tìm tín hiệu
        SSD1306_GotoXY(0, 25);
        SSD1306_Puts("Searching for", &Font_7x10, SSD1306_COLOR_WHITE);
        SSD1306_GotoXY(0, 37);
        SSD1306_Puts("GPS signal...", &Font_7x10, SSD1306_COLOR_WHITE);
    }
    
    // Cập nhật hiển thị
    SSD1306_UpdateScreen();
}

// Tạo mã Plus Code đơn giản (đây là gần đúng, không phải thuật toán chính xác)
void Generate_Simple_PlusCode(Neo6M_GpsData* GpsData, char* plusCode, size_t size)
{
    // Đây là một cách tiếp cận đơn giản để tạo ra một "giả" Plus Code
    // Không thể tính toán Plus Code chính xác mà không có thư viện
    
    // Tính giá trị tạm cho mã
    int lat_val = (int)(fabs(GpsData->latitude) * 1000);
    int lon_val = (int)(fabs(GpsData->longitude) * 1000);
    
    // Tạo một mã giả tương tự Plus Code với format: XX11+YY22
    // (đây không phải là Plus Code thực sự)
    snprintf(plusCode, size, "%c%c%02d+%c%c%02d",
             65 + (lat_val % 26), 65 + ((lat_val / 26) % 26), lat_val % 100,
             65 + (lon_val % 26), 65 + ((lon_val / 26) % 26), lon_val % 100);
}

// Hàm hiển thị Plus Code trên OLED (mở rộng từ hàm trước)
void Display_PlusCode_On_OLED(Neo6M_GpsData* GpsData)
{
    char pluscode_str[20];
    char time_str[20];
    char date_str[20];
    char speed_str[20];
    
    // Khởi tạo màn hình OLED
    SSD1306_Init();
    SSD1306_Clear();
    
    // Hiển thị tiêu đề
    SSD1306_GotoXY(0, 0);
    SSD1306_Puts("GPS LOCATION", &Font_7x10, SSD1306_COLOR_WHITE);
    SSD1306_DrawLine(0, 11, 128, 11, SSD1306_COLOR_WHITE);
    
    // Kiểm tra độ chính xác của dữ liệu
    if (GpsData->latitude != 0.0 && GpsData->longitude != 0.0) {
        // Tạo mã Plus Code giả
        Generate_Simple_PlusCode(GpsData, pluscode_str, sizeof(pluscode_str));
        
        // Định dạng dữ liệu thời gian (từ HHMMSS.SS sang HH:MM:SS)
        int hours = (int)(GpsData->time / 10000.0);
        int mins = (int)((GpsData->time - hours * 10000.0) / 100.0);
        int secs = (int)(GpsData->time - hours * 10000.0 - mins * 100.0);
        snprintf(time_str, sizeof(time_str), "Time: %02d:%02d:%02d", hours, mins, secs);
        
        // Tốc độ (chuyển từ knots sang km/h)
        float speed_kmh = GpsData->Knots * 1.852;  // 1 knot = 1.852 km/h
        snprintf(speed_str, sizeof(speed_str), "Speed: %.1f km/h", speed_kmh);
        
        // Hiển thị vị trí dạng Plus Code
        SSD1306_GotoXY(5, 20);
        SSD1306_Puts("Your Location:", &Font_7x10, SSD1306_COLOR_WHITE);
        
        SSD1306_GotoXY(15, 32);
        SSD1306_Puts(pluscode_str, &Font_11x18, SSD1306_COLOR_WHITE);
        
        SSD1306_GotoXY(0, 52);
        SSD1306_Puts(time_str, &Font_7x10, SSD1306_COLOR_WHITE);
    } else {
        // Hiển thị thông báo đang tìm tín hiệu
        SSD1306_GotoXY(0, 25);
        SSD1306_Puts("Searching for", &Font_7x10, SSD1306_COLOR_WHITE);
        SSD1306_GotoXY(0, 37);
        SSD1306_Puts("GPS signal...", &Font_7x10, SSD1306_COLOR_WHITE);
    }
    
    // Cập nhật hiển thị
    SSD1306_UpdateScreen();
}
