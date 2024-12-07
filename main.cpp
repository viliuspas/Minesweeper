#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <ctime>
#include "minesweeper.h"

#define GRID_SIZE 10
#define CELL_SIZE 30
#define NUM_MINES 10
#define TOOLBAR_HEIGHT 50
#define BORDER_WIDTH 20
#define WINDOW_HEIGHT GRID_SIZE + TOOLBAR_HEIGHT

using namespace std;

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
void Painter(HWND);
void PaintGrid(HDC);
void PaintValues(HDC);
void InitializeGrid(int, int);
void RevealCells(int, int);
bool areNeighbors(int, int, int , int);
void PrintGrid();

TCHAR szClassName[ ] = _T("Minesweeper");

struct Cell {
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int adjacentMines;
};

vector<vector<Cell> > grid(GRID_SIZE, vector<Cell>(GRID_SIZE));
HWND gridButtons[GRID_SIZE][GRID_SIZE];
bool isGridInitialized = false;

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (hThisInstance, MAKEINTRESOURCE(ID_ICON));
    wincl.hIconSm = LoadIcon (hThisInstance, MAKEINTRESOURCE(ID_ICON));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);

    if (!RegisterClassEx (&wincl))
        return 0;

   hwnd = CreateWindowEx(
        0,
        szClassName,
        _T("Minesweeper"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        GRID_SIZE * CELL_SIZE + 16 + BORDER_WIDTH * 2,
        GRID_SIZE * CELL_SIZE + 36 + TOOLBAR_HEIGHT + BORDER_WIDTH,
        HWND_DESKTOP,
        NULL,
        hThisInstance,
        NULL);

    ShowWindow (hwnd, nCmdShow);

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HINSTANCE hInstance;

    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

       case WM_CREATE:
            hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
            for (int x = 0; x < GRID_SIZE; ++x)
            {
                for (int y = 0; y < GRID_SIZE; ++y)
                {
                    gridButtons[x][y] = CreateWindow(
                        _T("BUTTON"),
                        _T(""),
                        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                        (x + ((double)(BORDER_WIDTH + 1) / (double)CELL_SIZE)) * CELL_SIZE,
                        (y + ((double)(TOOLBAR_HEIGHT + 1) / (double)CELL_SIZE)) * CELL_SIZE,
                        CELL_SIZE,
                        CELL_SIZE,
                        hwnd,
                        (HMENU)(x * GRID_SIZE + y),
                        hInstance,
                        NULL);
                }
            }
            break;

        case WM_COMMAND:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                int buttonID = LOWORD(wParam);
                int x = buttonID / GRID_SIZE;
                int y = buttonID % GRID_SIZE;

                if (!isGridInitialized)
                {
                    InitializeGrid(x, y);
                    isGridInitialized = true;
                }
                RevealCells(x, y);
            }
            break;

        case WM_PAINT:
        {
            Painter(hwnd);
            break;
        }

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

void Painter(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    PaintGrid(hdc);
    PaintValues(hdc);

    EndPaint(hwnd, &ps);
}

void PaintGrid(HDC hdc)
{
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
    SelectObject(hdc, hPen);

    for (int i = 0; i <= GRID_SIZE; ++i)
    {
        // Vertical lines
        double verticalFromX = i * CELL_SIZE + BORDER_WIDTH;
        double verticalFromY = TOOLBAR_HEIGHT;
        double verticalToX = i * CELL_SIZE  + BORDER_WIDTH;
        double verticalToY = GRID_SIZE * CELL_SIZE + TOOLBAR_HEIGHT;

        MoveToEx(hdc, verticalFromX, verticalFromY, NULL);
        LineTo(hdc, verticalToX, verticalToY);

        // Horizontal lines
        double horizontalFromX = BORDER_WIDTH;
        double horizontalFromY = i * CELL_SIZE + TOOLBAR_HEIGHT;
        double horizontalToX = GRID_SIZE * CELL_SIZE + BORDER_WIDTH;
        double horizontalToY = i * CELL_SIZE + TOOLBAR_HEIGHT;

        MoveToEx(hdc, horizontalFromX, horizontalFromY, NULL);
        LineTo(hdc, horizontalToX, horizontalToY);
    }

    DeleteObject(hPen);
}

void PaintValues(HDC hdc)
{
    RECT rect;
    HFONT hfont = CreateFont(CELL_SIZE, 0, 0, 0, 0, 0, 0, 0, OEM_CHARSET, 0, 0, 0, 0, TEXT("Terminal"));

    HGDIOBJ oldFont = SelectObject(hdc, hfont);
    SetBkMode(hdc, TRANSPARENT);

    for (int x = 0; x < GRID_SIZE; ++x)
    {
        for (int y = 0; y < GRID_SIZE; ++y)
        {
            SetRect(&rect,
                    x * CELL_SIZE + BORDER_WIDTH + 1,
                    y * CELL_SIZE + TOOLBAR_HEIGHT,
                    (x + 1) * CELL_SIZE + BORDER_WIDTH + 1,
                    (y + 1) * CELL_SIZE + TOOLBAR_HEIGHT);

            if (grid[x][y].isMine)
            {
                SetTextColor(hdc, RGB(0, 0, 0));
                DrawText(hdc, TEXT("M"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                continue;
            }

            switch (grid[x][y].adjacentMines)
            {
                case 1:
                    SetTextColor(hdc, RGB(0, 0, 253)); // Blue
                    DrawText(hdc, TEXT("1"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    break;
                case 2:
                    SetTextColor(hdc, RGB(1, 126, 0)); // Green
                    DrawText(hdc, TEXT("2"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    break;
                case 3:
                    SetTextColor(hdc, RGB(254, 0, 1)); // Red
                    DrawText(hdc, TEXT("3"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    break;
                case 4:
                    SetTextColor(hdc, RGB(1, 1, 128)); // Dark Blue
                    DrawText(hdc, TEXT("4"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    break;
                case 5:
                    SetTextColor(hdc, RGB(129, 1, 1)); // Maroon
                    DrawText(hdc, TEXT("5"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    break;
                case 6:
                    SetTextColor(hdc, RGB(0, 128, 128)); // Teal
                    DrawText(hdc, TEXT("6"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    break;
                case 7:
                    SetTextColor(hdc, RGB(0, 0, 0)); // Black
                    DrawText(hdc, TEXT("7"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    break;
                case 8:
                    SetTextColor(hdc, RGB(128, 128, 128)); // Gray
                    DrawText(hdc, TEXT("8"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    break;
                default:
                    break;
            }
        }
    }

    SelectObject(hdc, oldFont);
    DeleteObject(hfont);
}


void InitializeGrid(int startX, int startY)
{
    srand((unsigned)time(NULL));
    int minesPlaced = 0;
    while (minesPlaced < NUM_MINES)
    {
        int x = rand() % GRID_SIZE;
        int y = rand() % GRID_SIZE;
        if (!grid[x][y].isMine && !areNeighbors(startX, startY, x, y))
        {

            grid[x][y].isMine = true;
            ++minesPlaced;
        }
    }

    for (int x = 0; x < GRID_SIZE; ++x)
    {
        for (int y = 0; y < GRID_SIZE; ++y)
        {
            if (grid[x][y].isMine) continue;
            int mineCount = 0;
            for (int dx = -1; dx <= 1; ++dx)
            {
                for (int dy = -1; dy <= 1; ++dy)
                {
                    int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && ny >= 0 && nx < GRID_SIZE && ny < GRID_SIZE && grid[nx][ny].isMine)
                    {
                        ++mineCount;
                    }
                }
            }
            grid[x][y].adjacentMines = mineCount;
        }
    }
}

void RevealCells(int x, int y)
{
    if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE) return;
    if (grid[x][y].isRevealed || grid[x][y].isFlagged) return;

    grid[x][y].isRevealed = true;
    ShowWindow(gridButtons[x][y], SW_HIDE);

    if (grid[x][y].adjacentMines != 0 || grid[x][y].isMine) return;

    for (int nx = x - 1; nx <= x + 1; ++nx)
    {
        for (int ny = y - 1; ny <= y + 1; ++ny)
        {
            if (nx == x && ny == y) continue;
            RevealCells(nx, ny);
        }
    }
}

bool areNeighbors(int x1, int y1, int x2, int y2)
{
    for (int nx = x1 - 1; nx <= x1 + 1; ++nx)
    {
        for (int ny = y1 - 1; ny <= y1 + 1; ++ny)
        {
            if (nx < 0 || ny < 0 || nx >= GRID_SIZE || ny >= GRID_SIZE) continue;
            if (nx == x2 && ny == y2)
                return true;
        }
    }
    return false;
}

void PrintGrid()
{
    for(int y = 0; y < GRID_SIZE; ++y)
    {
        for(int x = 0; x < GRID_SIZE; ++x)
        {
            if (grid[x][y].isMine)
            {
                cout<<"M ";
                continue;
            }
            cout<<grid[x][y].adjacentMines<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
}

