//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include <vector>
#include "math.h"

void ShowBitmap(int x, int y, int x1, int y1, HBITMAP hBitmapBall, bool alpha = false);

// секция данных игры  
struct creature {
    HBITMAP hBitmap;
    float x, y, size, speed, reproduction_rate, growth_rate, nutritional_value, velocity, prevalence, age_limit;
    
    int age;
    int breeding_period;

    void load(const char* name)
    {
        hBitmap = (HBITMAP)LoadImageA(NULL, name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    }

    void show()
    {
        float amp = age;
        ShowBitmap(x,y-amp,size,amp, hBitmap);
    }

} ;

std::vector<creature>plant;


void processPlant()
{

    for (int i = 0;i < plant.size();i++)
    {
        plant[i].age++;

        if (plant[i].age > plant[i].age_limit)
        {
            plant.erase(plant.begin() + i);
        }
    }

    for (int i = 0;i < plant.size();i++)
    {
        if ((plant[i].age % plant[i].breeding_period) == plant[i].breeding_period - 1)
        {
            creature t;
            t = plant[i];
            int amp = 200;
            t.x += rand() % amp - amp/2;
            t.y += rand() % amp - amp/2;
            t.age = 0;

            bool isGrowingSpace = true;
            for (int j = 0;j < plant.size();j++)
            {
                float distance = sqrt(pow(t.x - plant[j].x, 2) + pow(t.y - plant[j].y, 2));
                if (distance < 20)
                {
                    isGrowingSpace = false;
                }

            }

            if (isGrowingSpace)
            {
                plant.push_back(t);
            }

        }
    }

}

struct {
    HWND hWnd;//хэндл окна
    HDC device_context, context;// два контекста устройства (для буферизации)
    int width, height;//сюда сохраним размеры окна которое создаст программа
} window;

HBITMAP hBack;// хэндл для фонового изображения

//cекция кода

void InitGame()
{
    hBack = (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    
    for (int j = 0; j < 3; j++)
    {
        srand(j*21122);
        int x = (rand() % window.width/2- window.width / 4) + window.width / 2;
        int y = (rand() % window.height/2 - window.height / 4 ) + window.height / 2;
        int size = window.width / 20;

        for (int i = 0; i < 3; i++)
        {
            creature t;
            t.load("plant.bmp");
            t.x = x+rand() % size;
            t.y = y+rand() % size;
            t.size = 20;
            t.age = rand() % 40;
            t.breeding_period = 110;
            t.age_limit = 117;
            plant.push_back(t);
        }
    }




    //------------------------------------------------------

         
   
}

void ShowBitmap(int x, int y, int x1, int y1, HBITMAP hBitmapBall, bool alpha )
{
    HBITMAP hbm, hOldbm;
    HDC hMemDC;
    BITMAP bm;

    hMemDC = CreateCompatibleDC(window.context); // Создаем контекст памяти, совместимый с контекстом отображения
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmapBall);// Выбираем изображение bitmap в контекст памяти

    if (hOldbm) // Если не было ошибок, продолжаем работу
    {
        GetObject(hBitmapBall, sizeof(BITMAP), (LPSTR)&bm); // Определяем размеры изображения

        if (alpha)
        {
            TransparentBlt(window.context, x, y, x1, y1, hMemDC, 0, 0, x1, y1, RGB(0, 0, 0));//все пиксели черного цвета будут интепретированы как прозрачные
        }
        else
        {
            StretchBlt(window.context, x, y, x1, y1, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY); // Рисуем изображение bitmap
        }

        SelectObject(hMemDC, hOldbm);// Восстанавливаем контекст памяти
    }

    DeleteDC(hMemDC); // Удаляем контекст памяти
}

void ShowRacketAndBall()
{
    ShowBitmap(0, 0, window.width, window.height, hBack);//задний фон

    for (int i = 0;i < plant.size();i++)
    {
        plant[i].show();
    }
    

}


void InitWindow()
{
    SetProcessDPIAware();
    window.hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//из хэндла окна достаем хэндл контекста устройства для рисования
    window.width = r.right - r.left;//определяем размеры и сохраняем
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//второй буфер
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//привязываем окно к контексту
    GetClientRect(window.hWnd, &r);

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    
    InitWindow();//здесь инициализируем все что нужно для рисования в окне
    InitGame();//здесь инициализируем переменные игры

   
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        processPlant();
        ShowRacketAndBall();//рисуем фон, ракетку и шарик

        for (int i = 0;i < plant.size();i++)
        {
            SetPixel(window.context, i, 0, RGB(255, 255, 255));
        }

        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)
    }

}
