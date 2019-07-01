#include "stdafx.h"
#include "ButtonUI.h"


ButtonUI::ButtonUI(HBITMAP bit, int posx, int posy)
{
	bitmap = bit;
	x = posx;
	y = posy;
	BITMAP tmpbit;
	GetObject(bitmap, sizeof(BITMAP), &tmpbit);
	width = tmpbit.bmWidth;
	height = tmpbit.bmHeight;
	x2 = x + width;
	y2 = y + height;
}

ButtonUI::~ButtonUI()
{
}

void ButtonUI::SetPosition(int posX, int posY)
{
	x = posX; 
	y = posY; 
	x2 = x + width;
	y2 = y + height;
}

bool ButtonUI::Collsion(int mx, int my)	//¸¶¿ì½º ÁÂÇ¥
{
	return (mx > x&&mx<x2&&my>y&&my < y2);
}
