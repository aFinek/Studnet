// MUEXE.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MUEXE.h"
#include "resource.h"
#include "overlay.h"
#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
// 此代码模块中包含的函数的前向声明: 
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPTSTR lpCmdLine,_In_ int nCmdShow)
{

	HWND hWnd;
	hInst = hInstance; // 将实例句柄存储在全局变量中
	hWnd = CreateDialog(hInstance, (LPCTSTR)IDD_DIALOG1, NULL, (DLGPROC)WndProc);
	//hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}
	MSG msg;
	// 设置对话框图标
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDR_MAINFRAME))); //IDI_ICON1为我自己载入的图标
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int) msg.wParam;
}
DWORD WINAPI 调试绘制(LPVOID 套接字)//循环绘图
{
	while (true)
	{
		render();
		Sleep(10);
	}
}
void 创建D3D绘图按扭(HWND hWnd)//按扭
{
	SetupWindow(hWnd);
	::CreateThread(0, 0, 调试绘制, NULL, 0, NULL);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	static HBRUSH hBrush;//刷子
	int  nCtrlId = 0;// LOWORD(lParam);//控件ID
	HDC hdc;
	nCtrlId = GetWindowLong((HWND)lParam, GWL_ID);//取控件ID
	switch (message)
	{
	case WM_INITDIALOG://消息是对话框才能收到的消息, 表明对话框及其所有子控件都创建完毕了。这个状态肯定是 调用显示对话框的函数之前
		//HWND hwndTab = GetDlgItem(hWnd, IDC_TAB1);//查询窗口句柄
		break;
	case WM_COMMAND:// 处理控件通知消息
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDOK:
			DestroyWindow(hWnd);
			break;
		case IDC_BUTTON1:
			创建D3D绘图按扭(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CTLCOLORSTATIC://更改Static控件的背景和字体
	{
							   wmId = LOWORD(wParam);
							   if (nCtrlId == IDC_CHECK1)
							   {
								   hBrush = CreateSolidBrush(RGB(0, 0, 0));  //该函数创建一个具有指定颜色的逻辑刷子。
								   SetTextColor((HDC)wParam, RGB(251, 216, 0));//设置static的前景色
								   SetBkMode((HDC)wParam, TRANSPARENT);//设置static的背景模式
								   return (BOOL)hBrush;//返回值为假的话，则不处理该消息，所有的操作无效，必须返回一个有效的hBrush来使设置生效
							   }
	}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return FALSE;
}