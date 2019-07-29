#pragma once
class ButtonUI {
public:
	ButtonUI(HBITMAP bit, int posx, int posy);
	~ButtonUI();
	HBITMAP bitmap;
	int x, y;
	int x2, y2;
	int width, height;
	virtual void SetPosition(int posX, int posY);
	virtual bool Collsion(int mx, int my);
	bool render = true;
};
class characterUI : public ButtonUI {
public:
	characterUI(HBITMAP bit, int posx, int poxy);
	~characterUI();
	wchar_t name[10] = L"TEST";
	bool master = false;
};
class RoomUI : public ButtonUI {
public:
	RoomUI(HBITMAP bit, int posx, int posy);
	~RoomUI();

	wchar_t name[10];
	int user_num = 0;
	int max_user = 8;
	int mode = 0;//0:팀전 1:개인전 2:대장전
	int room_num;
	bool playing = false;
};

class RoomMakeUI :public RoomUI {
public:
	RoomMakeUI(HBITMAP bit, int posx, int posy);
	~RoomMakeUI();
	virtual int Collision1(int mx, int my); //확인:1 취소:0
};
#define MODE_TEAM 0 
#define MODE_INDIVIDUAL 1
#define MODE_KING 2
struct RoomStatus {
	int max_user = 8;
	int mode = MODE_TEAM;
	int map = 0;
	int room_num = 0;
};