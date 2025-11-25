
struct {
    HWND hWnd;//хэндл окна
    HDC device_context, context;// два контекста устройства (для буферизации)
    int width, height;//сюда сохраним размеры окна которое создаст программа
} window;

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    SetProcessDPIAware();

    auto width = GetSystemMetrics(SM_CXSCREEN);
    auto height = GetSystemMetrics(SM_CYSCREEN);

    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, WndProc, 0,0, hInst, NULL, LoadCursor(NULL, IDC_ARROW), brush, NULL, L"fx", NULL };
    RegisterClassEx(&wcex);

    window.hWnd = CreateWindow(L"fx", L"fx", WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, width, height, NULL, NULL, hInst, NULL);
    if (!window.hWnd) return FALSE;

    ShowWindow(window.hWnd, SW_SHOW);
    UpdateWindow(window.hWnd);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);
    window.width = r.right - r.left;
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));

    hWnd = window.hWnd; 

    return TRUE;
}