#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Thư viện mã hóa Base32 cho Plus Code
const char BASE32_CHARS[] = "23456789CFGHJMPQRVWX";

// Hàm chuyển đổi tọa độ sang Plus Code đơn giản
// Lưu ý: Đây là phiên bản đơn giản hóa của thuật toán Plus Code
// Không đủ chính xác như thuật toán chính thức của Google
void LatLongToPlusCode(double lat, double lon, char* plusCode, size_t size)
{
    // Giới hạn trong phạm vi hợp lệ
    lat = fmin(90.0, fmax(-90.0, lat));
    lon = fmin(180.0, fmax(-180.0, lon));

    // Chuyển đổi vĩ độ và kinh độ từ -90/90 và -180/180 sang 0-180 và 0-360
    lat += 90.0;
    lon += 180.0;

    // Chia thành các ô có kích thước 20 độ (GRID_ROWS = 9, GRID_COLS = 18)
    int row = (int)(lat / 20.0);
    int col = (int)(lon / 20.0);

    // Ký tự đầu tiên của mã
    plusCode[0] = BASE32_CHARS[row];
    plusCode[1] = BASE32_CHARS[col];

    // Tính toán vị trí trong ô
    double lat_remainder = lat - (row * 20.0);
    double lon_remainder = lon - (col * 20.0);

    // Tinh chỉnh độ phân giải (mỗi ký tự tiếp theo chia nhỏ ô 20x20 hơn)
    double lat_resolution = 20.0;
    double lon_resolution = 20.0;

    for (int i = 2; i < 8; i += 2) {
        lat_resolution /= 20.0;
        lon_resolution /= 20.0;

        int lat_index = (int)(lat_remainder / lat_resolution);
        int lon_index = (int)(lon_remainder / lon_resolution);

        plusCode[i] = BASE32_CHARS[lat_index];
        plusCode[i+1] = BASE32_CHARS[lon_index];

        lat_remainder -= lat_index * lat_resolution;
        lon_remainder -= lon_index * lon_resolution;
    }

    // Thêm dấu + vào vị trí thứ 8
    plusCode[8] = '+';

    // Thêm 2 ký tự nữa cho độ chính xác cao hơn
    lat_resolution /= 20.0;
    lon_resolution /= 20.0;

    int lat_index = (int)(lat_remainder / lat_resolution);
    int lon_index = (int)(lon_remainder / lon_resolution);

    plusCode[9] = BASE32_CHARS[lat_index];
    plusCode[10] = BASE32_CHARS[lon_index];

    // Kết thúc chuỗi
    plusCode[11] = '\0';
}

// Chuyển đổi dữ liệu thời gian từ module GPS
void FormatGPSTime(double gps_time, char* time_str, size_t size)
{
    int hours = (int)(gps_time / 10000.0);
    int mins = (int)((gps_time - hours * 10000.0) / 100.0);
    int secs = (int)(gps_time - hours * 10000.0 - mins * 100.0);
    
    snprintf(time_str, size, "%02d:%02d:%02d", hours, mins, secs);
}

// Chuyển đổi dữ liệu ngày từ module GPS (DDMMYY)
void FormatGPSDate(double gps_date, char* date_str, size_t size)
{
    int day = (int)(gps_date / 10000.0);
    int month = (int)((gps_date - day * 10000.0) / 100.0);
    int year = (int)(gps_date - day * 10000.0 - month * 100.0) + 2000; // Giả sử năm 20xx
    
    snprintf(date_str, size, "%02d/%02d/%04d", day, month, year);
}

// Hàm tạo URL ngắn cho Google Maps
void GenerateShortMapURL(double lat, double lon, char* url, size_t size)
{
    // Format với độ chính xác 6 chữ số thập phân
    snprintf(url, size, "https://goo.gl/maps/%d%d",
             (int)(fabs(lat) * 1000000) % 1000000,
             (int)(fabs(lon) * 1000000) % 1000000);
}
