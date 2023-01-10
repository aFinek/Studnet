// MUEXE.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "MUEXE.h"
#include "resource.h"
#include "overlay.h"
#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
// �˴���ģ���а����ĺ�����ǰ������: 
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPTSTR lpCmdLine,_In_ int nCmdShow)
{

	HWND hWnd;
	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����
	hWnd = CreateDialog(hInstance, (LPCTSTR)IDD_DIALOG1, NULL, (DLGPROC)WndProc);
	//hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}
	MSG msg;
	// ���öԻ���ͼ��
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDR_MAINFRAME))); //IDI_ICON1Ϊ���Լ������ͼ��
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	// ����Ϣѭ��: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int) msg.wParam;
}
DWORD WINAPI ���Ի���(LPVOID �׽���)//ѭ����ͼ
{
	while (true)
	{
		render();
		Sleep(10);
	}
}
void ����D3D��ͼ��Ť(HWND hWnd)//��Ť
{
	SetupWindow(hWnd);
	::CreateThread(0, 0, ���Ի���, NULL, 0, NULL);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	static HBRUSH hBrush;//ˢ��
	int  nCtrlId = 0;// LOWORD(lParam);//�ؼ�ID
	HDC hdc;
	nCtrlId = GetWindowLong((HWND)lParam, GWL_ID);//ȡ�ؼ�ID
	switch (message)
	{
	case WM_INITDIALOG://��Ϣ�ǶԻ�������յ�����Ϣ, �����Ի����������ӿؼ�����������ˡ����״̬�϶��� ������ʾ�Ի���ĺ���֮ǰ
		//HWND hwndTab = GetDlgItem(hWnd, IDC_TAB1);//��ѯ���ھ��
		break;
	case WM_COMMAND:// ����ؼ�֪ͨ��Ϣ
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��: 
		switch (wmId)
		{
		case IDOK:
			DestroyWindow(hWnd);
			break;
		case IDC_BUTTON1:
			����D3D��ͼ��Ť(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CTLCOLORSTATIC://����Static�ؼ��ı���������
	{
							   wmId = LOWORD(wParam);
							   if (nCtrlId == IDC_CHECK1)
							   {
								   hBrush = CreateSolidBrush(RGB(0, 0, 0));  //�ú�������һ������ָ����ɫ���߼�ˢ�ӡ�
								   SetTextColor((HDC)wParam, RGB(251, 216, 0));//����static��ǰ��ɫ
								   SetBkMode((HDC)wParam, TRANSPARENT);//����static�ı���ģʽ
								   return (BOOL)hBrush;//����ֵΪ�ٵĻ����򲻴������Ϣ�����еĲ�����Ч�����뷵��һ����Ч��hBrush��ʹ������Ч
							   }
	}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  �ڴ���������ͼ����...
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