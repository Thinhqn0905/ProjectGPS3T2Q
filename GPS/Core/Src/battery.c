#include "battery.h"
#include "ssd1306.h"

extern I2C_HandleTypeDef hi2c1;
void Verti_draw_line(int start_x,int end_x,int start_y,int end_y)
{
	for(int x_b=start_y;x_b<=end_y;x_b++)
	{
			ssd1306_DrawPixel(start_x,x_b,White);
	}
}
void Horiz_draw_line(int start_x,int end_x,int start_y,int end_y)
{
	for(int x_b=start_x;x_b<=end_x;x_b++)
	{
			ssd1306_DrawPixel(x_b,start_y,White);
	}
}

void draw_cell(void)
{
	Horiz_draw_line(106,126,0,0);
	Verti_draw_line(106,106,0,10);
	Horiz_draw_line(106,126,10,10);
	Verti_draw_line(126,126,0,10);
	ssd1306_UpdateScreen(&hi2c1);
}

void show_Bat_voltage(float V_bat)
{
	double level=0.0f;
	draw_cell();
	level=(V_bat*25)+43.5;
	for(int i=106;i<=level;i++)
	{
		for(int j=0;j<=10;j++)
		{
			ssd1306_DrawPixel(i,j,White);
		}
	}	
	for(int i=level+1;i<126;i++)
	{
		for(int j=1;j<10;j++)
		{
			ssd1306_DrawPixel(i,j,Black);
		}
	}
	ssd1306_UpdateScreen(&hi2c1);
	
}
