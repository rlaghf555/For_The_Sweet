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

bool ButtonUI::Collsion(int mx, int my)	//마우스 좌표
{
	return (mx > x&&mx<x2&&my>y&&my < y2);
}

RoomUI::RoomUI(HBITMAP bit, int posx, int posy) : ButtonUI(bit, posx, posy)
{

}

RoomUI::~RoomUI()
{
}

RoomMakeUI::RoomMakeUI(HBITMAP bit, int posx, int posy) : RoomUI(bit, posx, posy)
{
}

RoomMakeUI::~RoomMakeUI()
{
}

int RoomMakeUI::Collision1(int mx, int my)
{
	int yesx = x + 75, yesx2 = x + 165;
	int nox = x + 200, nox2 = x + 290;
	int tmpy = y + 75, tmpy2 = y + 115;

	if (mx > yesx&&mx<yesx2&&my>tmpy&&my < tmpy2) {
		cout << "확인" << endl;
		return 1;
	}
	if (mx > nox&&mx<nox2&&my>tmpy&&my < tmpy2) {
		cout << "취소" << endl;
		return 0;
	}
}
