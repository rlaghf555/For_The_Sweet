// LabProject13.cpp: 응용 프로그램의 진입점을 정의합니다.
//

//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#include "stdafx.h"
#include "ForTheSweet.h"
#include "CGameFramework.h"
#include "ButtonUI.h"

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#pragma comment(lib, "msimg32.lib")

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

CGameFramework gGameFramework;
thread t;

bool threadRuning = true;
int state = STATE_LOGIN;	//인게임 여부 확인
wchar_t IP_Adress[15];
wchar_t ID[15];

HBITMAP LogIn_Image, Menu_Image, Room_Image, Loading_Image;
ButtonUI *Button_Start, *Button_Exit, *Button_MakeRoom, *Button_Left, *Button_Right;
RoomUI *Button_Room[6];
RoomMakeUI *Room_Make;
ButtonUI *Mode_Left, *Mode_Right, *Num_Left, *Num_Right, *Map_Left, *Map_Right, *Button_Team1, *Button_Team2, *Room_Out, *Room_GameStart;
ButtonUI *skill, *selecting, *attack, *speed, *health;
ButtonUI *fix;
ButtonUI *battleroyale;
characterUI *character[8];

ButtonUI *outline;//외곽선
ButtonUI *crown;//왕관
int selected_skill = 0; //0 공격력, 1 이동속도 , 2 체력회복
RoomStatus myroom;

HWND hMainWnd, hGameWnd;

bool text_id = true;	//true면 ID입력 false면 IP입력
bool makeroom = false;
bool game_ready = false;

char * WCtoC(wchar_t* str)
{
	//반환할 char* 변수 선언
	char* pStr;

	//입력받은 wchar_t 변수의 길이를 구함
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	//char* 메모리 할당
	pStr = new char[strSize];

	//형 변환 
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
	return pStr;
}

wchar_t* CtoWC(char* str)
{
	//wchar_t형 변수 선언
	wchar_t* pStr;
	//멀티 바이트 크기 계산 길이 반환
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);

	//wchar_t 메모리 할당
	pStr = new WCHAR[strSize];
	//형 변환
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);

	return pStr;
}

void processPacket(char *ptr)
{
	switch (ptr[1])
	{
	case SC_ROOM_INFO:
	{
		sc_packet_room_info *p_room_info;
		p_room_info = reinterpret_cast<sc_packet_room_info*>(ptr);

		wchar_t *name = CtoWC(p_room_info->name);

		wcscpy_s(Button_Room[0]->name, _countof(Button_Room[0]->name), name);
		Button_Room[0]->mode = MODE_INDIVIDUAL;
		Button_Room[0]->user_num = p_room_info->current_num;
		Button_Room[0]->room_num = p_room_info->room_num;

		break;
	}
	case SC_ROOM_DETAIL_INFO:
	{
		sc_packet_room_detail_info *p_room_detail_info;
		p_room_detail_info = reinterpret_cast<sc_packet_room_detail_info*>(ptr);

		wchar_t *name = CtoWC(p_room_detail_info->name);
		myroom.room_num = p_room_detail_info->room_num;

		break;
	}
	default:
	{
		gGameFramework.processPacket(ptr);
		break;
	}
	}
}

void recvCallBack()
{
	while (threadRuning)
	{
		int retval;

		//while(m_pSocket == NULL);

		retval = recv(gGameFramework.m_pSocket->clientSocket, gGameFramework.m_pSocket->buf, MAX_PACKET_SIZE, 0);

		if (retval == 0 || retval == SOCKET_ERROR)
		{
			closesocket(gGameFramework.m_pSocket->clientSocket);
		}

		char *ptr = gGameFramework.m_pSocket->buf;
		//cout << "Packet Len : " << retval << endl;

		while (retval > 0)
		{
			if (gGameFramework.m_pSocket->packet_size == 0) gGameFramework.m_pSocket->packet_size = ptr[0];
			if (gGameFramework.m_pSocket->saved_size + retval >= gGameFramework.m_pSocket->packet_size) {
				memcpy(gGameFramework.m_pSocket->packetBuffer + gGameFramework.m_pSocket->saved_size, ptr,
					gGameFramework.m_pSocket->packet_size - gGameFramework.m_pSocket->saved_size);

				processPacket(gGameFramework.m_pSocket->packetBuffer);

				ptr += gGameFramework.m_pSocket->packet_size - gGameFramework.m_pSocket->saved_size;
				retval -= gGameFramework.m_pSocket->packet_size - gGameFramework.m_pSocket->saved_size;
				gGameFramework.m_pSocket->packet_size = 0;
				gGameFramework.m_pSocket->saved_size = 0;
			}
			else {
				memcpy(gGameFramework.m_pSocket->packetBuffer + gGameFramework.m_pSocket->saved_size, ptr, retval);
				gGameFramework.m_pSocket->saved_size += retval;
				retval = 0;
			}
			//cout << "size : " << int(size) << "type : " << int(type) << endl;
		}
	}
}

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_FORTHESWEET, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FORTHESWEET));

	MSG msg;
	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT && state == STATE_MENU) break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		if (state == STATE_GAME) {
			gGameFramework.FrameAdvance();
		}
		else if (state == STATE_LOADING) {
			gGameFramework.selected_map = myroom.map;
			gGameFramework.SetIDIP(ID, IP_Adress);
			gGameFramework.OnCreate(hInst, hGameWnd);

			state = STATE_GAME;
		}
	}
	
	threadRuning = false;

	//gGameFramework.OnDestroy();
	SoundManager::GetInstance()->Release();
	if(SERVER_ON)
		t.join();

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
void DrawBitmap(HDC hdc, int x, int y, HBITMAP &hBit) {
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;

	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;
	BitBlt(hdc, x, y, bx, by, MemDC, x, y, SRCCOPY);
	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}
void DrawButton(HDC hdc, ButtonUI *button) {
	HDC MemDC;
	HBITMAP OldBitmap;

	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, button->bitmap);


	TransparentBlt(hdc, button->x, button->y, button->width, button->height, MemDC, 0, 0, button->width, button->height, RGB(255, 255, 255));
	//BitBlt(hdc, x, y, bx, by, MemDC, x, y, SRCCOPY);
	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}
void DrawCharacter(HDC hdc, characterUI *charater) {
	if (!charater->render)
		return;
	HDC MemDC;
	HBITMAP OldBitmap;
	HFONT myFont;
	HFONT oldFont;
	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, charater->bitmap);
	TransparentBlt(hdc, charater->x, charater->y, charater->width, charater->height, MemDC, 0, 0, charater->width, charater->height, RGB(255, 255, 255));

	myFont = CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, FF_ROMAN, NULL);
	oldFont = (HFONT)SelectObject(hdc, myFont);
	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, charater->x + 85, charater->y + 17, charater->name, lstrlen(charater->name));
	SelectObject(hdc, oldFont);
	DeleteObject(myFont);

	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
	if (charater->master) {
		crown->SetPosition(charater->x+150, charater->y-10);
		DrawButton(hdc, crown);
	}
}
void DrawRoom(HDC hdc, RoomUI *room) {
	if (!room->render)
		return;
	HDC MemDC;
	HBITMAP OldBitmap;
	HFONT myFont;
	HFONT oldFont;
	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, room->bitmap);
	TransparentBlt(hdc, room->x, room->y, room->width, room->height, MemDC, 0, 0, room->width, room->height, RGB(255, 255, 255));

	myFont = CreateFont(30, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, FF_ROMAN, NULL);
	oldFont = (HFONT)SelectObject(hdc, myFont);
	wchar_t str[1];
	SetBkMode(hdc, TRANSPARENT);
	SetTextAlign(hdc, TA_CENTER);
	TextOut(hdc, room->x + 180, room->y + 25, room->name, lstrlen(room->name));
	SetTextAlign(hdc, TA_LEFT);
	_itow(room->user_num, str, 10);
	TextOut(hdc, room->x + 260, room->y + 77, str, lstrlen(str));	//현재인원
	_itow(room->max_user, str, 10);
	TextOut(hdc, room->x + 310, room->y + 77, str, lstrlen(str));	//최대인원
	SelectObject(hdc, oldFont);
	DeleteObject(myFont);

	myFont = CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, FF_ROMAN, NULL);
	oldFont = (HFONT)SelectObject(hdc, myFont);
	if (room->mode == 0)
		TextOut(hdc, room->x + 40, room->y + 85, L"팀전", lstrlen(L"팀전"));
	if (room->mode == 1)
		TextOut(hdc, room->x + 28, room->y + 85, L"개인전", lstrlen(L"개인전"));
	if (room->mode == 2)
		TextOut(hdc, room->x + 28, room->y + 85, L"대장전", lstrlen(L"대장전"));
	SelectObject(hdc, oldFont);
	DeleteObject(myFont);
	if (room->playing) {
		outline->SetPosition(room->x, room->y);
		DrawButton(hdc, outline);
	}
	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}
void DrawRoommake(HDC hdc, RoomMakeUI *room) {
	HDC MemDC;
	HBITMAP OldBitmap;
	HFONT myFont;
	HFONT oldFont;
	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, room->bitmap);
	TransparentBlt(hdc, room->x, room->y, room->width, room->height, MemDC, 0, 0, room->width, room->height, RGB(255, 255, 255));

	myFont = CreateFont(30, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, FF_ROMAN, NULL);
	oldFont = (HFONT)SelectObject(hdc, myFont);
	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, room->x + 28, room->y + 25, room->name, lstrlen(room->name));
	SelectObject(hdc, oldFont);
	DeleteObject(myFont);
	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}

void LoadBitMap() {
	//로그인 UI
	LogIn_Image = (HBITMAP)LoadImage(NULL, L"resource\\image\\LogIn_Image2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	//메인메뉴 UI
	Menu_Image = (HBITMAP)LoadImage(NULL, L"resource\\image\\Menu.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Room_Image = (HBITMAP)LoadImage(NULL, L"resource\\image\\Roombackground.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Loading_Image = (HBITMAP)LoadImage(NULL, L"resource\\image\\Loading.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Button_Start = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\gamestart.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 540, 618);
	Button_Exit = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\exit.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 540, 623);
	Button_MakeRoom = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\room.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 540, 525);
	Button_Left = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\left.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 438, 529);
	Button_Right = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\right.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 774, 529);
	for (int i = 0; i < 2; i++)
		Button_Room[i] = new RoomUI((HBITMAP)LoadImage(NULL, L"resource\\image\\RoomUI.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 264 + i * 370, 118);
	for (int i = 2; i < 4; i++)
		Button_Room[i] = new RoomUI((HBITMAP)LoadImage(NULL, L"resource\\image\\RoomUI.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 264 + (i - 2) * 370, 250);
	for (int i = 4; i < 6; i++)
		Button_Room[i] = new RoomUI((HBITMAP)LoadImage(NULL, L"resource\\image\\RoomUI.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 264 + (i - 4) * 370, 392);
	outline = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\outline.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 0, 0);
	Room_Make = new RoomMakeUI((HBITMAP)LoadImage(NULL, L"resource\\image\\makeroom.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 450, 250);

	fix = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\fix.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 874, 529);

	//방 안 UI
	Mode_Left = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\left.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 35, 183);
	Map_Left = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\left.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 35, 331);
	Num_Left = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\left.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 35, 474);
	Mode_Right = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\right.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 357, 183);
	Map_Right = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\right.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 357, 331);
	Num_Right = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\right.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 357, 474);

	Button_Team1 = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\Team1_Open.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 571, 107);
	Button_Team2 = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\Team2_Open.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 958, 107);
	battleroyale = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\battleroyale.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 630, 107);
	Room_Out = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\out.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 861, 620);
	Room_GameStart = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\gamestart.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 1069, 620);

	skill = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\skill.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 571, 430);
	selecting = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\select.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 851, 430);
	attack = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\attack.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 900, 500);
	speed = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\speed.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 990, 500);
	health = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\health.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 1081, 500);
	crown = new ButtonUI((HBITMAP)LoadImage(NULL, L"resource\\image\\crown.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 0, 0);
	for (int i = 0; i < 4;i++) {
		character[i] = new characterUI((HBITMAP)LoadImage(NULL, L"resource\\image\\name.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 591, 190+i*55);
	}
	for (int i = 4; i < 8; i++) {
		character[i] = new characterUI((HBITMAP)LoadImage(NULL, L"resource\\image\\name.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE), 981, 190+(i-4)*55);
	}

	for (int i = 0; i < 8; i++)
		character[i]->render = false;

	character[0]->render = true;
	character[0]->master = true;

	character[1]->render = true;

	wcscpy(character[0]->name, L"-MASTER-");

	for (int i = 0; i < 6; i++)
		Button_Room[i]->render = false;
	Button_Room[0]->render = true;
	Button_Room[1]->render = true;
	Button_Room[1]->playing = true;

	wcscpy(Button_Room[0]->name, L"TESTROOM");
	wcscpy(Button_Room[1]->name, L"PLAYING");
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_FORTHESWEET));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;	// 주윈도우 메뉴 없앰 - MAKEINTRESOURCEW(IDC_LabProject13);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	//hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rc, dwStyle, FALSE); 
	hMainWnd = CreateWindowW(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
	hInst = hInstance;
	SoundManager::GetInstance()->Setup();
	SoundManager::GetInstance()->PlayBackGroundSounds(SOUND_1);
	if (UI_ON) {
		LoadBitMap();
	}
	else {
		state = STATE_GAME;

		if (!SERVER_ON) {
			state = STATE_GAME;
			gGameFramework.OnCreate(hInst, hMainWnd);
		}
		else if (SERVER_ON) {
			gGameFramework.OnCreate(hInst, hMainWnd);
			t = thread(recvCallBack);
		}
	}

	if (!hMainWnd)
	{
		return FALSE;
	}

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HFONT myFont;
	HFONT oldFont;
	int mx, my;
	if (state == STATE_GAME) {	//게임 시작 후 
		switch (message)
		{
		case WM_SIZE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_KEYDOWN:
		case WM_KEYUP:
			gGameFramework.OnProcessingWindowMessage(hGameWnd, message, wParam, lParam);
			if ((LOWORD(wParam) == VK_DELETE)) {
				// 게임 강제종료
				gGameFramework.OnDestroy();
				//ShowWindow(hGameWnd, SW_HIDE);
				//메인메뉴 재시작
				ShowWindow(hMainWnd, SW_SHOWDEFAULT);
				UpdateWindow(hMainWnd);
				InvalidateRect(hMainWnd, NULL, FALSE);
				state = STATE_ROOM;

				//RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
				//DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
				//AdjustWindowRect(&rc, dwStyle, FALSE);
				//
				//HWND newwindow = CreateWindowW(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);
				//hMainWnd = newwindow;
				//ShowWindow(hMainWnd, 10);
				//UpdateWindow(hMainWnd);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	else if (state == STATE_LOGIN) {
		switch (message)
		{
		case WM_SIZE:
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			if (mx > 450 && mx < 830 && my>450 && my < 505) {		//ID입력
				text_id = true;
			}
			else if (mx > 450 && mx < 830 && my>550 && my < 605) {	//IP입력
				text_id = false;
			}
			if (Button_Start->Collsion(mx, my))
			{	//게임 시작
				if (SERVER_ON)
				{
					gGameFramework.SetIDIP(ID, IP_Adress);

					gGameFramework.m_pSocket = new CSocket(gGameFramework.m_pid, gGameFramework.m_pip);
					if (gGameFramework.m_pSocket) {
						if (gGameFramework.m_pSocket->init())
						{
							t = thread(recvCallBack);

							cs_packet_connect p_connect;
							p_connect.type = CS_CONNECT;
							p_connect.size = sizeof(cs_packet_connect);
							strcpy_s(p_connect.id, _countof(p_connect.id), gGameFramework.m_pid);

							send(gGameFramework.m_pSocket->clientSocket, (char *)&p_connect, sizeof(cs_packet_connect), 0);
						}
					}
				}
				state = STATE_MENU;
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_RBUTTONDOWN:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_MOUSEMOVE:
			break;
		case WM_KEYDOWN:

			if (text_id) {
				if ((LOWORD(wParam) >= 'A' && LOWORD(wParam) <= 'Z') || (LOWORD(wParam) >= '0' && LOWORD(wParam) <= '9')) {
					if (lstrlen(ID) < 15) {
						if (LOWORD(wParam) == 190)
							ID[lstrlen(ID)] = '.';
						else
							ID[lstrlen(ID)] = LOWORD(wParam);
					}
				}
				if (LOWORD(wParam) == 8) {
					if (lstrlen(ID) > 0)
						ID[lstrlen(ID) - 1] = '\0';
				}

			}
			else {
				if ((LOWORD(wParam) >= '0' && LOWORD(wParam) <= '9') || LOWORD(wParam) == 190) { //190: '.'
					if (lstrlen(IP_Adress) < 15) {
						if (LOWORD(wParam) == 190)
							IP_Adress[lstrlen(IP_Adress)] = '.';
						else
							IP_Adress[lstrlen(IP_Adress)] = LOWORD(wParam);
					}

				}
				if (LOWORD(wParam) == 8) {
					if (lstrlen(IP_Adress) > 0)
						IP_Adress[lstrlen(IP_Adress) - 1] = '\0';
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_KEYUP:
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			DrawBitmap(hdc, 0, 0, LogIn_Image);
			DrawButton(hdc, Button_Start);
			myFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
			oldFont = (HFONT)SelectObject(hdc, myFont);
			TextOut(hdc, 500, 463, ID, lstrlen(ID));
			TextOut(hdc, 500, 563, IP_Adress, lstrlen(IP_Adress));
			SelectObject(hdc, oldFont);
			DeleteObject(myFont);
			EndPaint(hWnd, &ps);

			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	else if (state == STATE_MENU) {
		switch (message)
		{
			
		case WM_SIZE:
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			InvalidateRect(hWnd, NULL, FALSE);
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			if (makeroom) {
				if (Room_Make->Collision1(mx, my) == 1) {
					makeroom = false;
					state = STATE_ROOM;

					if (SERVER_ON)
					{
						char* room_name = WCtoC(Room_Make->name);

						cs_packet_make_room p_make_room;
						p_make_room.size = sizeof(cs_packet_make_room);
						p_make_room.type = CS_MAKE_ROOM;
						strcpy_s(p_make_room.name, _countof(p_make_room.name), room_name);

						send(gGameFramework.m_pSocket->clientSocket, (char *)&p_make_room, sizeof(cs_packet_make_room), 0);
					}
					break;
				}
				if (Room_Make->Collision1(mx, my) == 0) {
					makeroom = false;
					break;
				}
			}
			if (makeroom == false) {
				if (Button_MakeRoom->Collsion(mx, my)) { //방만들기 
					cout << "방만들기" << endl;
					makeroom = true;
					break;
				}
				if (Button_Exit->Collsion(mx, my)) { // 종료
					cout << "종료" << endl;
					//exit(1);
					PostQuitMessage(0);
					break;
				}
				if (Button_Left->Collsion(mx, my)) { //방목록 왼쪽으로 넘기기
					cout << "방목록 왼쪽으로 넘기기" << endl;
					break;
				}
				if (Button_Right->Collsion(mx, my)) { //방목록 오른쪽으로 넘기기
					cout << "방목록 오른쪽으로 넘기기" << endl;
					break;
				}
				if (fix->Collsion(mx, my)) {
					cout << "방목록 새로고침" << endl;
					break;
				}
				for (int i = 0; i < 6; i++)
				{
					if (Button_Room[i]->Collsion(mx, my)) { //임시 방 버튼 (이후 포문으로 체크)
						cout << "방입장" << i << endl;

						if (SERVER_ON)
						{
							int room_num = Button_Room[i]->room_num;

							cs_packet_attend_room p_attend_room;
							p_attend_room.size = sizeof(cs_packet_attend_room);
							p_attend_room.type = CS_ATTEND_ROOM;
							p_attend_room.room_num = room_num;

							send(gGameFramework.m_pSocket->clientSocket, (char *)&p_attend_room, sizeof(cs_packet_attend_room), 0);
						}

						state = STATE_ROOM;
						break;
					}
				}
			}
			break;
		case WM_RBUTTONDOWN:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_MOUSEMOVE:
			break;
		case WM_KEYDOWN:
			if (makeroom) {
				if ((LOWORD(wParam) >= 'A' && LOWORD(wParam) <= 'Z') || (LOWORD(wParam) >= '0' && LOWORD(wParam) <= '9')) {
					if (lstrlen(Room_Make->name) < 10) {
						if (LOWORD(wParam) == 190)
							Room_Make->name[lstrlen(Room_Make->name)] = '.';
						else
							Room_Make->name[lstrlen(Room_Make->name)] = LOWORD(wParam);
					}
				}
				if (LOWORD(wParam) == 8) {
					if (lstrlen(Room_Make->name) > 0)
						Room_Make->name[lstrlen(Room_Make->name) - 1] = '\0';
				}

			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_KEYUP:
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			DrawBitmap(hdc, 0, 0, Menu_Image);
			DrawButton(hdc, Button_MakeRoom);
			DrawButton(hdc, Button_Exit);
			DrawButton(hdc, Button_Left);
			DrawButton(hdc, Button_Right);
			DrawButton(hdc, fix);
			for (int i = 0; i < 6; i++)
				DrawRoom(hdc, Button_Room[i]);
			if (makeroom)
				DrawRoommake(hdc, Room_Make);
			EndPaint(hWnd, &ps);

			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	else if (state == STATE_ROOM) {
		switch (message)
		{
		case WM_SIZE:
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			InvalidateRect(hWnd, NULL, FALSE);
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			if (Mode_Left->Collsion(mx, my)) {
				cout << "모드 왼쪽으로 변경" << endl;
				if (myroom.map == 0|| myroom.map==2) {
					if (myroom.mode == MODE_INDIVIDUAL)
						myroom.mode = MODE_TEAM;
					else if (myroom.mode == MODE_TEAM)
						myroom.mode = MODE_INDIVIDUAL;
				}
				break;
			}
			if (Mode_Right->Collsion(mx, my)) {
				cout << "모드 오른쪽으로 변경" << endl;
				if (myroom.map == 0 || myroom.map == 2) {
					if (myroom.mode == MODE_INDIVIDUAL)
						myroom.mode = MODE_TEAM;
					else if (myroom.mode == MODE_TEAM)
						myroom.mode = MODE_INDIVIDUAL;
				}
			}
			if (Num_Left->Collsion(mx, my)) {
				cout << "최대인원 왼쪽으로 변경" << endl;
				myroom.max_user -= 1;
				if (myroom.max_user < 2)
					myroom.max_user = 8;
				break;
			}
			if (Num_Right->Collsion(mx, my)) {
				cout << "최대인원 오른쪽으로 변경" << endl;
				myroom.max_user += 1;
				if (myroom.max_user > 8)
					myroom.max_user = 2;
				break;
			}
			if (Map_Left->Collsion(mx, my)) {
				cout << "맵 왼쪽으로 변경" << endl;
				myroom.map -= 1;
				if (myroom.map < 0)
					myroom.map = 2;
				if (myroom.map == 1)
					myroom.mode = MODE_KING;
				else myroom.mode = MODE_TEAM;
				break;
			}
			if (Map_Right->Collsion(mx, my)) {
				cout << "맵 오른쪽으로 변경" << endl;
				myroom.map += 1;
				if (myroom.map > 2)
					myroom.map = 0;
				if (myroom.map == 1)
					myroom.mode = MODE_KING;
				else myroom.mode = MODE_TEAM;
				break;
			}
			if (Button_Team1->Collsion(mx, my)) {
				cout << "팀1로 변경" << endl;

				break;
			}
			if (Button_Team2->Collsion(mx, my)) {
				cout << "팀2로 변경" << endl;

				break;
			}
			if (Room_Out->Collsion(mx, my)) {
				cout << "방 나가기" << endl;
				state = STATE_MENU;

				break;
			}
			if (Room_GameStart->Collsion(mx, my)) {
				hdc = BeginPaint(hWnd, &ps);
				DrawBitmap(hdc, 0, 0, Loading_Image);
				EndPaint(hWnd, &ps);
				cout << "게임시작" << endl;

				if (SERVER_ON)
				{
					cs_packet_start_room p_start_room;
					p_start_room.size = sizeof(cs_packet_attend_room);
					p_start_room.type = CS_START_ROOM;
					p_start_room.room_num = myroom.room_num;

					send(gGameFramework.m_pSocket->clientSocket, (char *)&p_start_room, sizeof(cs_packet_start_room), 0);
				}

				RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
				DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
				AdjustWindowRect(&rc, dwStyle, FALSE);
				hGameWnd = CreateWindowW(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);
				state = STATE_LOADING;
				ShowWindow(hGameWnd, SW_SHOWDEFAULT);
				ShowWindow(hMainWnd, SW_HIDE);

				UpdateWindow(hGameWnd);
				InvalidateRect(hGameWnd, NULL, FALSE);

				break;
			}

			if (attack->Collsion(mx, my)) {
				selected_skill = 0;
				InvalidateRect(hGameWnd, NULL, FALSE);
				break;
			}
			if (speed->Collsion(mx, my)) {
				selected_skill = 1;
				InvalidateRect(hGameWnd, NULL, FALSE);
				break;
			}
			if (health ->Collsion(mx, my)) {
				selected_skill = 2;
				InvalidateRect(hGameWnd, NULL, FALSE);
				break;
			}
			break;
		case WM_RBUTTONDOWN:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_MOUSEMOVE:
			break;
		case WM_KEYDOWN:
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_KEYUP:
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			DrawBitmap(hdc, 0, 0, Room_Image);
			DrawButton(hdc, Mode_Left);
			DrawButton(hdc, Num_Left);
			DrawButton(hdc, Map_Left);
			DrawButton(hdc, Mode_Right);
			DrawButton(hdc, Num_Right);
			DrawButton(hdc, Map_Right);
			if (myroom.mode == MODE_TEAM || myroom.mode == MODE_KING) {
				DrawButton(hdc, Button_Team1);
				DrawButton(hdc, Button_Team2);
			}
			else {
				DrawButton(hdc, battleroyale);
			}

			DrawButton(hdc, skill);
			DrawButton(hdc, selecting);

			if (selected_skill == 0) {
				attack->SetPosition(640, 500);
				DrawButton(hdc, attack);
				attack->SetPosition(900, 500);

			}
			else if (selected_skill == 1) {
				speed->SetPosition(640, 500);
				DrawButton(hdc, speed);
				speed->SetPosition(990, 500);
			}
			else if (selected_skill == 2) {
				health->SetPosition(640, 500);
				DrawButton(hdc, health);
				health->SetPosition(1081, 500);
			}
			DrawButton(hdc, attack);
			DrawButton(hdc, speed);
			DrawButton(hdc, health);
			SetTextAlign(hdc, TA_CENTER);
			for (int i = 0; i < 8; i++) {
				DrawCharacter(hdc, character[i]);
			}
			SetTextAlign(hdc, TA_LEFT);

			DrawButton(hdc, Room_Out);
			DrawButton(hdc, Room_GameStart);


			myFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
			oldFont = (HFONT)SelectObject(hdc, myFont);
			SetBkMode(hdc, TRANSPARENT);
			if (myroom.mode == 0)
				TextOut(hdc, 200, 210, L"팀전", lstrlen(L"팀전"));
			if (myroom.mode == 1)
				TextOut(hdc, 190, 210, L"개인전", lstrlen(L"개인전"));
			if (myroom.mode == 2)
				TextOut(hdc, 190, 210, L"대장전", lstrlen(L"대장전"));


			if (myroom.map == 0) {
				wchar_t str[4] = L"웨하스";
				TextOut(hdc, 190, 355, str, lstrlen(str));	//맵
			}
			if (myroom.map == 1) {
				wchar_t str[4] = L"오레오";
				TextOut(hdc, 190, 355, str, lstrlen(str));	//맵
			}
			if (myroom.map == 2) {
				wchar_t str[4] = L"케이크";
				TextOut(hdc, 190, 355, str, lstrlen(str));	//맵
			}
			wchar_t str1[1];
			_itow(myroom.max_user, str1, 10);
			TextOut(hdc, 225, 500, str1, lstrlen(str1));	//최대인원



			SelectObject(hdc, oldFont);
			DeleteObject(myFont);
			EndPaint(hWnd, &ps);

			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	else if (state == STATE_LOADING) {
		switch (message)
		{
		case WM_CREATE:
			hdc = BeginPaint(hGameWnd, &ps);
			DrawBitmap(hdc, 0, 0, Loading_Image);
			EndPaint(hGameWnd, &ps);
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			break;
		case WM_KEYDOWN:
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hGameWnd, &ps);
			DrawBitmap(hdc, 0, 0, Loading_Image);
			EndPaint(hGameWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
