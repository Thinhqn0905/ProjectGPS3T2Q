#include "ssd1306.h"
#include <string.h>
#include <stdlib.h>


// Screenbuffer
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Screen object
SSD1306_t SSD1306;

// Private I2C handle
static I2C_HandleTypeDef *SSD1306_I2C_PORT;

// SSD1306 commands
#define SSD1306_COMMAND 0x00
#define SSD1306_DATA 0x40

// SSD1306 Display commands
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_CHARGEPUMP 0x8D

// Additional commands
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7

// Function prototypes
static void ssd1306_WriteCommand(uint8_t command);
static void ssd1306_WriteData(uint8_t* data, uint16_t size);

//
// Original library functions
//

uint8_t ssd1306_Init(I2C_HandleTypeDef *hi2c) {
    SSD1306_I2C_PORT = hi2c;

    // Wait for the screen to boot
    HAL_Delay(100);

    /* Init LCD */
    ssd1306_WriteCommand(SSD1306_DISPLAYOFF);
    ssd1306_WriteCommand(SSD1306_SETDISPLAYCLOCKDIV);
    ssd1306_WriteCommand(0x80);
    ssd1306_WriteCommand(SSD1306_SETMULTIPLEX);
    ssd1306_WriteCommand(0x3F);
    ssd1306_WriteCommand(SSD1306_SETDISPLAYOFFSET);
    ssd1306_WriteCommand(0x00);
    ssd1306_WriteCommand(SSD1306_SETSTARTLINE | 0x00);
    ssd1306_WriteCommand(SSD1306_CHARGEPUMP);
    ssd1306_WriteCommand(0x14);
    ssd1306_WriteCommand(SSD1306_MEMORYMODE);
    ssd1306_WriteCommand(0x00);
    ssd1306_WriteCommand(SSD1306_SEGREMAP | 0x01);
    ssd1306_WriteCommand(SSD1306_COMSCANDEC);
    ssd1306_WriteCommand(SSD1306_SETCOMPINS);
    ssd1306_WriteCommand(0x12);
    ssd1306_WriteCommand(SSD1306_SETCONTRAST);
    ssd1306_WriteCommand(0xCF);
    ssd1306_WriteCommand(SSD1306_SETPRECHARGE);
    ssd1306_WriteCommand(0xF1);
    ssd1306_WriteCommand(SSD1306_SETVCOMDETECT);
    ssd1306_WriteCommand(0x40);
    ssd1306_WriteCommand(SSD1306_DISPLAYALLON_RESUME);
    ssd1306_WriteCommand(SSD1306_NORMALDISPLAY);
    ssd1306_WriteCommand(SSD1306_DISPLAYON);

    // Clear screen
    ssd1306_Fill(Black);

    // Update screen
    ssd1306_UpdateScreen(hi2c);

    // Set default values for screen object
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;

    SSD1306.Initialized = 1;

    return 1;
}

void ssd1306_UpdateScreen(I2C_HandleTypeDef *hi2c) {
    uint8_t i;

    for (i = 0; i < 8; i++) {
        ssd1306_WriteCommand(0xB0 + i);
        ssd1306_WriteCommand(0x00);
        ssd1306_WriteCommand(0x10);

        ssd1306_WriteData(&SSD1306_Buffer[SSD1306_WIDTH * i], SSD1306_WIDTH);
    }
}

void ssd1306_Fill(SSD1306_COLOR color) {
    uint32_t i;

    for(i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        return;
    }

    if(color == White) {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } else {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

void ssd1306_SetCursor(uint8_t x, uint8_t y) {
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}

char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color) {
    uint32_t i, b, j;

    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;

    // Check remaining space on current line
    if (SSD1306_WIDTH < (SSD1306.CurrentX + Font.FontWidth) ||
        SSD1306_HEIGHT < (SSD1306.CurrentY + Font.FontHeight))
    {
        // Not enough space on current line
        return 0;
    }

    // Use the font to write
    for(i = 0; i < Font.FontHeight; i++) {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for(j = 0; j < Font.FontWidth; j++) {
            if((b << j) & 0x8000)  {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
            } else {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
            }
        }
    }

    // The current space is now taken
    SSD1306.CurrentX += Font.FontWidth;

    // Return written char for validation
    return ch;
}

char ssd1306_WriteString(const char* str, FontDef Font, SSD1306_COLOR color) {
    while (*str) {
        if (ssd1306_WriteChar(*str, Font, color) != *str) {
            // Char could not be written
            return *str;
        }
        str++;
    }

    // Everything ok
    return *str;
}

void ssd1306_InvertColors(void) {
    SSD1306.Inverted = !SSD1306.Inverted;
    ssd1306_WriteCommand(SSD1306.Inverted ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
}

//
// Wrapper functions to match existing project implementation
//

uint8_t SSD1306_Init(void) {
    // Assuming hi2c1 is used in the project
    extern I2C_HandleTypeDef hi2c1;
    return ssd1306_Init(&hi2c1);
}

void SSD1306_UpdateScreen(void) {
    // Assuming hi2c1 is used in the project
    extern I2C_HandleTypeDef hi2c1;
    ssd1306_UpdateScreen(&hi2c1);
}

void SSD1306_Clear(void) {
    ssd1306_Fill(Black);
}

void SSD1306_GotoXY(uint8_t x, uint8_t y) {
    ssd1306_SetCursor(x, y);
}

char SSD1306_Puts(const char* str, FontDef* Font, SSD1306_COLOR color) {
    // Dereference the pointer to get the actual font
    return ssd1306_WriteString(str, *Font, color);
}

void SSD1306_ON(void) {
    ssd1306_WriteCommand(SSD1306_DISPLAYON);
}

void SSD1306_OFF(void) {
    ssd1306_WriteCommand(SSD1306_DISPLAYOFF);
}

//
// Private functions
//
void SSD1306_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, SSD1306_COLOR color) {
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        ssd1306_DrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

static void ssd1306_WriteCommand(uint8_t command) {
    uint8_t dt[2] = { SSD1306_COMMAND, command };
    HAL_I2C_Master_Transmit(SSD1306_I2C_PORT, SSD1306_I2C_ADDR, dt, 2, HAL_MAX_DELAY);
}

static void ssd1306_WriteData(uint8_t* data, uint16_t size) {
    uint8_t *buffer = (uint8_t*)malloc(size + 1);
    if (buffer) {
        buffer[0] = SSD1306_DATA;
        memcpy(buffer + 1, data, size);
        HAL_I2C_Master_Transmit(SSD1306_I2C_PORT, SSD1306_I2C_ADDR, buffer, size + 1, HAL_MAX_DELAY);
        free(buffer);
    }
}
