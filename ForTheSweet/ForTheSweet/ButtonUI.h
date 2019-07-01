#pragma once
class ButtonUI {
public:
	ButtonUI(HBITMAP bit, int posx, int posy);
	~ButtonUI();
	HBITMAP bitmap;
	int x, y;
	int x2, y2;
	int width, height;
	void SetPosition(int posX, int posY); 
	bool Collsion(int mx, int my);
};