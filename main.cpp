#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include <d3d9.h>
#include <tchar.h>
#include <iostream>

#pragma execution_character_set("utf-8")

// 全局变量
static HWND hwnd;
static HWND GameHwnd;
static RECT WindowRectangle;
static int WindowWide, WindowHeight;

static LPDIRECT3D9 g_pD3D = NULL;
static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

// 单选框设置状态
bool show_another_window = false;

// imgui 回调函数
// By: LyShark
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    {
        return false;
    }

    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
    {
        return false;
    }
    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
    }

    if (g_pD3D)
    {
        g_pD3D->Release();
        g_pD3D = NULL;
    }
}
// https://www.cnblogs.com/lyshark
void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
    {
        IM_ASSERT(0);
    }
    ImGui_ImplDX9_CreateDeviceObjects();
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
        {
            return 0;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// 绘制主方法
// lyshark.com
void DrawImGUI()
{
    // 启动IMGUI自绘
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    static float f = 0.0f;
    static int counter = 0;
    static char sz[256] = { 0 };

    ImGui::Begin("LyShark 辅助GUI主菜单");
    ImGui::Text("这是一段测试字符串");
    ImGui::Checkbox("弹出子窗口", &show_another_window);
    ImGui::SliderFloat("浮点条", &f, 0.0f, 1.0f);

    ImGui::InputText("输入内容", sz, 256, 0, 0, 0);

    if (ImGui::Button("点我触发"))
        counter++;
    ImGui::SameLine();
    ImGui::Text("触发次数 = %d", counter);

    ImGui::Text("当前FPS: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    if (show_another_window)
    {
        ImGui::Begin("我是子窗体", &show_another_window);
        ImGui::Text(" 您好,LyShark !");
        if (ImGui::Button("关闭窗体"))
            show_another_window = false;
        ImGui::End();
    }
    ImGui::EndFrame();
}

// 自身窗口循环事件
void WindowMessageLoop()
{
    bool done = false;
    while (!done)
    {
        // 每次都将窗体置顶并跟随游戏窗体移动
        GetWindowRect(GameHwnd, &WindowRectangle);
        WindowWide = (WindowRectangle.right) - (WindowRectangle.left);
        WindowHeight = (WindowRectangle.bottom) - (WindowRectangle.top);
        DWORD dwStyle = GetWindowLong(GameHwnd, GWL_STYLE);
        if (dwStyle & WS_BORDER)
        {
            WindowRectangle.top += 23;
            WindowHeight -= 23;
        }

        // 跟随窗口移动
        MoveWindow(hwnd, WindowRectangle.left + 8, WindowRectangle.top + 8, WindowWide - 11, WindowHeight - 11, true);

        // 开始消息循环
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
            {
                done = true;
            }
        }

        if (done)
        {
            break;
        }

        // 开始绘制
        DrawImGUI();

        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        {
            ResetDevice();
        }
    }
}

int main(int argc, char* argv[])
{
    // 注册窗体类
    WNDCLASSEX wc;

    // 附加到整个屏幕上
    wc.cbClsExtra = NULL;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = NULL;
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(0, IDI_APPLICATION);
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.lpszClassName = L" ";
    wc.lpszMenuName = L" ";
    wc.style = CS_VREDRAW | CS_HREDRAW;
    ::RegisterClassEx(&wc);

    // 屏幕宽度和高度
    WindowWide = GetSystemMetrics(SM_CXSCREEN);
    WindowHeight = GetSystemMetrics(SM_CYSCREEN);

    // 创建窗体
    HWND hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW, L" ", L" ", WS_POPUP, 1, 1, WindowWide, WindowHeight, 0, 0, wc.hInstance, 0);

    // 显示窗口
    SetLayeredWindowAttributes(hwnd, 0, 1.0f, LWA_ALPHA);
    SetLayeredWindowAttributes(hwnd, 0, RGB(0, 0, 0), LWA_COLORKEY);
    ShowWindow(hwnd, SW_SHOW);

    // 初始化D3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 0;
    }

    // 更新窗体
    UpdateWindow(hwnd);

    // 初始化ImGUI
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 13.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // 开始执行绘制循环事件
    WindowMessageLoop();

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}
