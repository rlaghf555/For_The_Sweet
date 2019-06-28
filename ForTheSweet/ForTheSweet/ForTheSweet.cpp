// LabProject13.cpp: 응용 프로그램의 진입점을 정의합니다.
//

//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#include "stdafx.h"
#include "ForTheSweet.h"
#include "CGameFramework.h"

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

CGameFramework gGameFramework;
thread t;

bool threadRuning = true;
bool flag = false;
bool gamestart = true;	//인게임 여부 확인
wchar_t IP_Adress[15];
wchar_t ID[15];

HBITMAP LogIn_Image;
bool text_id = true;	//true면 ID입력 false면 IP입력
void recvCallBack()
{
	while (threadRuning)
	{
		//if(flag)
		gGameFramework.recvCallBack();
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

	// 기본 메시지 루프입니다.
	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else if(gamestart){
			gGameFramework.FrameAdvance();
		}
		else {

		}
	}
	threadRuning = false;

	gGameFramework.OnDestroy();

	if(SERVER_ON)
		t.join();

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit) {
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;

	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;
	BitBlt(hdc, 0, 0, bx, by, MemDC, 0, 0, SRCCOPY);
	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
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
	HWND hMainWnd = CreateWindowW(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
	LogIn_Image = (HBITMAP)LoadImage(NULL, L"resource\\image\\LogIn_Image2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	gGameFramework.OnCreate(hInstance, hMainWnd);

	if (SERVER_ON) {
		t = thread(recvCallBack);
		flag = true;
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
	HDC hdc,memdc;
	HBITMAP OldBitmap;
	PAINTSTRUCT ps;
	HFONT myFont;
	HFONT oldFont;
	int mx, my;
	if (gamestart) {	//게임 시작 후 
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
			gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	else {
		switch (message)
		{
		case WM_SIZE:
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			mx=LOWORD(lParam);
			my=HIWORD(lParam);
				if (mx > 450 && mx < 830 && my>450 && my < 505) {		//ID입력
					text_id = true;
				}
				else if (mx > 450 && mx < 830 && my>550 && my < 605) {	//IP입력
					text_id = false;
				}
			break;
		case WM_RBUTTONDOWN:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_MOUSEMOVE:
			break;
		case WM_KEYDOWN:

			if (text_id) {
				if ((LOWORD(wParam) >= 'a' && LOWORD(wParam) <= 'z') || (LOWORD(wParam) >= '0' && LOWORD(wParam) <= '9')) {
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
