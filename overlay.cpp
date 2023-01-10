#include "stdafx.h"
#include "overlay.h"
#include <string>
#include <cstdarg>


int s_width;
int s_height;

//ESP stuff
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;
HWND hWnd;
HWND twnd;
MARGINS  margin;
LPD3DXFONT pFont=0;
LPD3DXFONT pFont1 = 0;
ID3DXLine* d3dLine;

void SetupWindow(HWND nwnd)//��װ��ͼ
{
	RECT rc;
	/* 
	while(!twnd)
	twnd = FindWindow(L"UnrealWindow", 0);//��ȡָ�����ھ��
	if (twnd != NULL)
	{
	GetWindowRect(twnd, &rc);
	s_width = rc.right - rc.left;
	s_height = rc.bottom - rc.top;
	}
	else
	{
	cout << "Closing..." << GetLastError() << endl;
	Sleep(3000);
	Shutdown();
	ExitProcess(0);
	}
	*/
	twnd = nwnd;
	if (twnd != NULL)
	{
		GetWindowRect(twnd, &rc);
		s_width = rc.right - rc.left;
		s_height = rc.bottom - rc.top;
	}
	WNDCLASSEXW wc;
	
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(0);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	wc.lpszClassName = (LPCWSTR)"crappy";
	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, (LPCWSTR)"", WS_POPUP, rc.left, rc.top, s_width, s_height, NULL, NULL, wc.hInstance, NULL);

	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_COLORKEY);//LWA_COLORKEY=͸��
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);//�������
	ShowWindow(hWnd, SW_SHOW);
	initD3D(hWnd);
}




void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

	D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
	d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;     // set the back buffer format to 32-bit
	d3dpp.BackBufferWidth = s_width;    // set the width of the buffer
	d3dpp.BackBufferHeight = s_height;    // set the height of the buffer

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);


	D3DXCreateFontA(d3ddev, 12, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "����", &pFont);//��������
	//D3DXCreateFontA(d3ddev, 20, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "΢���ź�", &pFont1);//��������
	D3DXCreateLine(d3ddev, &d3dLine);
}

void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...)//�����ı�
{
	char buf[1024] = { 0 };
	va_list va_alist;
	RECT FontPos = { x, y, x + 120, y + 16 };
	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);
	
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}

void DrawLine(float x, float y, float xx, float yy, D3DCOLOR color)//����ֱ��
{
	D3DXVECTOR2 dLine[2];

	d3dLine->SetWidth(1.f);

	dLine[0].x = x;
	dLine[0].y = y;

	dLine[1].x = xx;
	dLine[1].y = yy;

	d3dLine->Draw(dLine, 2, color);

}

void DrawBox(float x, float y, float width, float height, int m_width,D3DCOLOR color)//���ƾ���
{
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	d3dLine->SetWidth(m_width);//���
	d3dLine->Draw(points, 5, color);
}



LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		//render();
		break;
	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &margin);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
int x=0;
void render()
{
	// clear the window alpha  �������alpha
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // //��ʼ��Ⱦ

	//calculate and and draw esp stuff   ����ͻ���ESP����
	//ESP();
	DrawString((int)x, (int)10, D3DCOLOR_ARGB(25, 42, 224, 42), pFont, "[����: %d]");//�ı�
	DrawBox(x, 30, 100, 100, 3,D3DCOLOR_ARGB(255, 42, 224, 42));//����
	//DrawLine(1, 50, 100, 50, D3DCOLOR_ARGB(255, 250, 255, 0));//ֱ��
	x++;
	if (x>s_width)
	{
		x = 0;
	}
	
	d3ddev->EndScene();//������Ⱦ

	d3ddev->Present(NULL, NULL, NULL, NULL);   //��ʾ displays the created frame on the screen ����Ļ����ʾ�����Ŀ��
}