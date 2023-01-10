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

void SetupWindow(HWND nwnd)//安装绘图
{
	RECT rc;
	/* 
	while(!twnd)
	twnd = FindWindow(L"UnrealWindow", 0);//获取指定窗口句柄
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
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_COLORKEY);//LWA_COLORKEY=透明
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);//设置最顶层
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


	D3DXCreateFontA(d3ddev, 12, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "宋体", &pFont);//创建字体
	//D3DXCreateFontA(d3ddev, 20, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "微软雅黑", &pFont1);//创建字体
	D3DXCreateLine(d3ddev, &d3dLine);
}

void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...)//绘制文本
{
	char buf[1024] = { 0 };
	va_list va_alist;
	RECT FontPos = { x, y, x + 120, y + 16 };
	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);
	
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}

void DrawLine(float x, float y, float xx, float yy, D3DCOLOR color)//绘制直线
{
	D3DXVECTOR2 dLine[2];

	d3dLine->SetWidth(1.f);

	dLine[0].x = x;
	dLine[0].y = y;

	dLine[1].x = xx;
	dLine[1].y = yy;

	d3dLine->Draw(dLine, 2, color);

}

void DrawBox(float x, float y, float width, float height, int m_width,D3DCOLOR color)//绘制矩形
{
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	d3dLine->SetWidth(m_width);//宽度
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
	// clear the window alpha  清除窗口alpha
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // //开始渲染

	//calculate and and draw esp stuff   计算和绘制ESP材料
	//ESP();
	DrawString((int)x, (int)10, D3DCOLOR_ARGB(25, 42, 224, 42), pFont, "[距离: %d]");//文本
	DrawBox(x, 30, 100, 100, 3,D3DCOLOR_ARGB(255, 42, 224, 42));//矩形
	//DrawLine(1, 50, 100, 50, D3DCOLOR_ARGB(255, 250, 255, 0));//直线
	x++;
	if (x>s_width)
	{
		x = 0;
	}
	
	d3ddev->EndScene();//结束渲染

	d3ddev->Present(NULL, NULL, NULL, NULL);   //显示 displays the created frame on the screen 在屏幕上显示创建的框架
}