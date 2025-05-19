#ifndef SSD1306_H
#define SSD1306_H

#include "stm32f4xx_hal.h"
#include "fonts.h"

// I2c address
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR 0x78
#endif // SSD1306_I2C_ADDR

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH 128
#endif // SSD1306_WIDTH

// SSD1306 LCD height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT 64
#endif // SSD1306_HEIGHT

#ifndef SSD1306_COM_LR_REMAP
#define SSD1306_COM_LR_REMAP 0
#endif // SSD1306_COM_LR_REMAP

#ifndef SSD1306_COM_ALTERNATIVE_PIN_CONFIG
#define SSD1306_COM_ALTERNATIVE_PIN_CONFIG 1
#endif // SSD1306_COM_ALTERNATIVE_PIN_CONFIG

// Define colors as per project implementation
#define SSD1306_COLOR_BLACK 0x00  // Black color, no pixel
#define SSD1306_COLOR_WHITE 0x01  // Pixel is set. Color depends on LCD

//
// Enumeration for screen colors (Keep for backward compatibility)
//
typedef enum {
    Black = SSD1306_COLOR_BLACK,
    White = SSD1306_COLOR_WHITE
} SSD1306_COLOR;

//
// Struct to store transformations
//
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
} SSD1306_t;

// Global handle to be used across the application
extern SSD1306_t SSD1306;

//
// Function definitions adapted to match project implementation
//

// Original functions for backward compatibility
uint8_t ssd1306_Init(I2C_HandleTypeDef *hi2c);
void ssd1306_UpdateScreen(I2C_HandleTypeDef *hi2c);
void ssd1306_Fill(SSD1306_COLOR color);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color);
char ssd1306_WriteString(const char* str, FontDef Font, SSD1306_COLOR color);
void ssd1306_SetCursor(uint8_t x, uint8_t y);
void ssd1306_InvertColors(void);

// New function names matching project implementation
uint8_t SSD1306_Init(void);
void SSD1306_UpdateScreen(void);
void SSD1306_Clear(void);  // Alias for ssd1306_Fill(Black)
void SSD1306_GotoXY(uint8_t x, uint8_t y);  // Alias for ssd1306_SetCursor
char SSD1306_Puts(const char* str, FontDef* Font, SSD1306_COLOR color);
void SSD1306_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, SSD1306_COLOR color);


// Helper functions
void SSD1306_ON(void);
void SSD1306_OFF(void);

#endif // SSD1306_H
