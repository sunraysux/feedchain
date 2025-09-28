#define _CRT_SECURE_NO_WARNINGS

const float PI = 3.1415926535897;
#include "framework.h"
#include "windows.h"
#include "timer.h"
#include "vector"
#include  "windowsx.h"
HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD currentTime;

HWND hWnd;

#include "MainWindow.h"

#include "resource.h"

#include "WICTextureLoader.h"
#include "GameConfig.h"
#include "Chunks.h"

#include "dx11.h"
#include "Classes.h"
#include "ecosystem.h"
static int prevMouseX = 0;
static int prevMouseY = 0;



float lerp(float x1, float x2, float a)
{
    return x1 * (1 - a) + x2 * a;
}

float length(float x1, float y1, float x2, float y2)
{
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

#include "Loop.h"
#define MAX_LOADSTRING 100
// current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DX11MINIMAL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }
    //InitWindow();  
    Dx11Init();
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DX11MINIMAL));

    MSG msg = { 0 };

    timer::StartCounter();

    Camera::HW();
    terraloop();
    //Camera::Camera();
    // Main message loop:
    while (msg.message != WM_QUIT)
    {
        double time = timer::GetCounter();

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }


        if (time >= timer::nextFrameTime)
        {
            currentTime = timer::GetCounter();
            
            timer::frameBeginTime = timer::GetCounter();
            drawWorld();
            //mainLoop();
            
            
            timer::frameEndTime = timer::GetCounter();
            timer::frameRenderingDuration = timer::frameEndTime - timer::frameBeginTime;
            timer::nextFrameTime = timer::frameBeginTime + FRAME_LEN;
            double frameInterval = timer::frameBeginTime - currentTime;
        }

        Sleep((DWORD)min(FRAME_LEN, max(FRAME_LEN - timer::frameRenderingDuration, 0)));
    }

    return (int)msg.wParam;
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DX11MINIMAL));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DX11MINIMAL);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
        if ((lParam & 0x40000000) == 0) // игнор автоповтора
        {
            bool shiftHeld = (GetAsyncKeyState(VK_SHIFT) & 0x8000);

            switch (wParam)
            {
            case VK_SHIFT:
                Camera::state.speedMul = 10.0f;
                
                break;
            case VK_SPACE:
                if (gameState == gameState_::game)
                    gameState = gameState_::pause;
                else if (gameState == gameState_::pause)
                    gameState = gameState_::game;
                break;

            case '1':
                if (shiftHeld) gameSpeed = 1;
                else currentType = type_::wolf, slot_number = 1;
                break;
            case '2':
                if (shiftHeld) gameSpeed = 2;
                else currentType = type_::rabbit, slot_number = 2;
                break;
            case '3':
                if (shiftHeld) gameSpeed = 3;
                else currentType = type_::tree, slot_number = 3;
                break;
            case '4':
                if (shiftHeld) gameSpeed = 4;
                else currentType = type_::bush, slot_number = 4;
                break;
            case '5':
                if (shiftHeld) gameSpeed = 5;
                else currentType = type_::eagle, slot_number = 5;
                break;
            case '6': currentType = type_::rat;slot_number = 6;       break;
            case '7': currentType = type_::grass;slot_number = 7; break;
            case '8': currentType = type_::lightning;slot_number = 8; break;

            break;
            case VK_ESCAPE:
              //  gameState = gameState_::MainMenu;
                PostQuitMessage(0);
                return 0;
            }
        }
        break;

    case WM_LBUTTONDOWN: {
        if ((lParam & 0x40000000) == 0) {
            float barPositions = -0.35;
            float barHeights = -0.85;
            float barBottom = -1;
            float barTop = barHeights;

            for (int slot = 1; slot < 9; slot++) {
                if ((barBottom < Camera::state.mousendcY && Camera::state.mousendcY < barHeights) &&
                    (barPositions - 0.1 + slot * 0.1 < Camera::state.mousendcX && Camera::state.mousendcX < barPositions + slot * 0.1)) {
                    slot_number = slot;
                }
            }
            switch (slot_number)
            {
            case 1: currentType = type_::wolf; break;
            case 2: currentType = type_::rabbit; break;
            case 3: currentType = type_::tree; break;
            case 4: currentType = type_::bush; break;
            case 5: currentType = type_::eagle; break;
            case 6: currentType = type_::rat; break;
            case 7: currentType = type_::grass; break;
            case 8: currentType = type_::lightning; break;
            default:
                break;
            }
        }
    }
    break;
    case WM_KEYUP:
        if (wParam == VK_SHIFT) {
            Camera::state.speedMul = 1.0f;
        }
        break;
    case WM_MOUSEWHEEL:
    {
        Camera::HandleMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        break;
    }
    case WM_MOUSEMOVE:
    {
        int currentX = GET_X_LPARAM(lParam);
        int currentY = GET_Y_LPARAM(lParam);

        if (wParam & MK_RBUTTON) // Правая кнопка - вращение
        {
            int dx = currentX - prevMouseX;
            int dy = currentY - prevMouseY;
            Camera::HandleMouseMovement(dx, dy, true);
        }
        else if (wParam & MK_MBUTTON) // Средняя кнопка - панорамирование
        {
            int dx = currentX - prevMouseX;
            int dy = currentY - prevMouseY;
            Camera::HandleMouseMovement(dx, dy, false);
        }

        // Обновляем предыдущую позицию
        prevMouseX = currentX;
        prevMouseY = currentY;
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_CLOSE:
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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
